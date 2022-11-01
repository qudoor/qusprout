#include <sstream>
#include "config/Config.h"
#include "common/qulog.h"
#include "common/Singleton.h"
#include "ResourceManager.h"
#include "interface/ecode_constants.h"
#include "taskmanager/TaskManager.h"

CResource::CResource()
{
    m_createtime = getCurrMs();
    m_updatetime = m_createtime;
}

void CResource::updateTime()
{
    m_updatetime = getCurrMs();
}


CResourceManager::CResourceManager()
{

}

CResourceManager::~CResourceManager()
{
}

//Init local resource info
bool CResourceManager::init()
{
    auto handle = std::make_shared<CResource>();
    if (!handle)
        return false;

    int ret = SINGLETON(CSystemHandle)->getMachineSysInfo(handle->m_resource.machine);
    if (ret != 0)
    {
        LOG(ERROR) << "getMachineSysInfo failed(ret:" << ret << ").";
        return false; 
    }

    ret = SINGLETON(CSystemHandle)->getMemUseInfo(handle->m_resource.resource);
    if (ret != 0)
    {
        LOG(ERROR) << "getMemUseInfo failed(ret:" << ret << ").";
        return false;
    }

    m_resourceList[handle->m_resource.machine.addr] = handle;

    return true;
}

ErrCode::type CResourceManager::getResource(const InitQubitsReq& req, std::string& resourceid, ResourceData& resourcebytes)
{
    std::map<std::string, ResourceData> resources;
    SINGLETON(CTaskManager)->getAllUseResourceBytes(resources);
    
    if (ExecCmdType::type::ExecTypeCpuMpi == req.exec_type)
    {
        return getMpiCpuResource(req, resources, resourceid, resourcebytes);
    }

    return getSignalCpuResource(req, resources, resourceid, resourcebytes);
}

ErrCode::type CResourceManager::getMpiCpuResource(const InitQubitsReq& req, const std::map<std::string, ResourceData>& useresource, std::string& resourceid, ResourceData& resourcebytes)
{
    std::lock_guard<std::mutex> guard(m_mutex);
    size_t ressize = m_resourceList.size();
    if (ressize == 0)
    {
        //无可用资源
        LOG(ERROR) << "no resource is available(taskId:" << req.id << ").";
        return ErrCode::type::COM_NOT_RESOURCE;
    }

    int numranks = 0;
    long long int mpibytes = 0;
    std::vector<std::string> mpicpuhosts;
    
    numranks = getNumRanks(req.qubits);
    mpibytes = calcMpiBytes(req.qubits, req.density, numranks);
    for (auto& temp : m_resourceList)
    {
        long long cpumemory = temp.second->m_resource.resource.totalRam;
        auto addr = temp.second->m_resource.machine.addr;
        auto finditer = useresource.find(addr);
        if (finditer != useresource.end())
        {
            cpumemory -= finditer->second.cpubytes;
        }
        if (cpumemory >= mpibytes)
        {
            mpicpuhosts.push_back(addr);
        }
    }

    size_t mpisize = mpicpuhosts.size();
    if (0 == mpisize && mpisize < (size_t)numranks)
    {
        LOG(ERROR) << "resource memory is not enough(taskId:" << req.id << ").";
        return ErrCode::type::COM_MEM_NOT_ENOUGH;
    }

    resourcebytes.cpubytes = mpibytes;
    int index = m_index++%mpisize;
    auto host = mpicpuhosts[index];
    resourceid = m_resourceList[host]->m_resource.machine.addr;

    return ErrCode::type::COM_SUCCESS;
}

ErrCode::type CResourceManager::getSignalCpuResource(const InitQubitsReq& req, const std::map<std::string, ResourceData>& useresource, std::string& resourceid, ResourceData& resourcebytes)
{
    std::lock_guard<std::mutex> guard(m_mutex);
    size_t ressize = m_resourceList.size();
    if (ressize == 0)
    {
        //无可用资源
        LOG(ERROR) << "no resource is available(taskId:" << req.id << ").";
        return ErrCode::type::COM_NOT_RESOURCE;
    }

    long long int singlebytes = calcSingleBytes(req.qubits, req.density);
    std::vector<std::string> cpuhosts;
    for (auto& temp : m_resourceList)
    {
        long long cpumemory = temp.second->m_resource.resource.totalRam;
        auto addr = temp.second->m_resource.machine.addr;
        auto finditer = useresource.find(addr);
        if (finditer != useresource.end())
        {
            cpumemory -= finditer->second.cpubytes;
        }
        if (cpumemory >= singlebytes)
        {
            cpuhosts.push_back(addr);
        }
    }

    size_t cpusize = cpuhosts.size();
    if (0 == cpusize)
    {
        LOG(ERROR) << "resource memory is not enough(taskId:" << req.id << ").";
        return ErrCode::type::COM_MEM_NOT_ENOUGH;
    }

    resourcebytes.cpubytes = singlebytes;
    auto index = m_index++%cpusize;
    auto host = cpuhosts[index];
    resourceid = m_resourceList[host]->m_resource.machine.addr;

    return ErrCode::type::COM_SUCCESS;
}

long long CResourceManager::calcSingleBytes(const int qubits, const int density)
{
    long long havebytes = 0;
    if (density)
    {
        havebytes = 16*1024*pow(2, qubits);
    }
    else
    {  
        havebytes = 16*pow(2, qubits);
    }
    return havebytes;
}

long long CResourceManager::calcMpiBytes(const int qubits, const int density, const int numranks)
{
    long long havebytes = 0;
    if (density)
    {
        havebytes = 32*1024*pow(2, qubits)/numranks;
    }
    else
    {  
        havebytes = 32*pow(2, qubits)/numranks;
    }
    return havebytes;
}

int CResourceManager::getNumRanks(const int qubits)
{
    int hostsize = 0;
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        hostsize = m_resourceList.size();
    }
    if (1 == hostsize)
    {
        return 1;
    }

    //取小于等于并且离hostsize最近的2的n次方
    long unsigned int numranks = 1;
    long unsigned int newnumranks = 1;
    while(newnumranks <= (long unsigned int)hostsize)
    {
        numranks = newnumranks;
        newnumranks *= 2;
    }
    
    //qubit太小的时候取2的qubits次方作为进程数
    long unsigned int numamps = (1UL<<qubits);
    if (numamps < numranks)
    {
        numranks = numamps;
    }

    return (int)numranks;
}

//获取所有的资源
void CResourceManager::getAllResource(std::map<std::string, DeviceDetail>& devlist)
{
    std::lock_guard<std::mutex> guard(m_mutex);
    auto iter = m_resourceList.begin();
    for (; iter != m_resourceList.end(); ++iter)
    {
        auto temp = iter->second->m_resource;

        MachineDetail machine;
        machine.__set_addr(temp.machine.addr);
        machine.__set_sys_name(temp.machine.sys_name);
        machine.__set_sys_release(temp.machine.sys_release);
        machine.__set_sys_version(temp.machine.sys_version);
        machine.__set_sys_machine(temp.machine.sys_machine);

        ResourceDetail resource;
        resource.__set_cpu_total_memory(temp.resource.totalRam);
        resource.__set_cpu_free_memory(temp.resource.freeRam);

        DeviceDetail device;
        device.__set_machine(machine);
        device.__set_resource(resource);

        devlist.insert(std::pair<std::string, DeviceDetail>(temp.machine.addr, device));
    }
}
