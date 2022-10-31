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

void CResource::updateResource(const RegisterReq& req)
{
    m_updatetime = getCurrMs();
    m_resource = req;
}

void CResource::updateResource(const HeartbeatReq& req)
{
    m_updatetime = getCurrMs();
    if (req.up_resource)
    {
        if (m_resource.device.resource.cpu_total_memory != req.device.resource.cpu_total_memory)
        {
            LOG(INFO) << "update resource(req:" << getPrint(req) << ").";
        }
        m_resource.__set_device(req.device);
    }
}

void CResource::getAddr(RpcConnectInfo& addr)
{
    addr.__set_addr(m_resource.rpc.addr);
    addr.__set_port(m_resource.rpc.port);
}

std::string CResource::getAddr()
{
    return m_resource.rpc.addr;
}

bool CResource::isTimeout()
{
    auto now = getCurrMs();
    if (now - m_updatetime > SINGLETON(CConfig)->m_resourceTimeOutDuration*1000)
    {
        return true;
    }

    return false;
}

void CResource::cleanResourceOfTask()
{
    SINGLETON(CTaskManager)->cleanResourceOfTask(m_resource.resource_id);
}

void CResource::updateTime()
{
    m_updatetime = getCurrMs();
}

bool CResource::isEqualResource(const std::string& addr)
{
    return m_resource.device.machine.addr == addr;
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
        retrun false;

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
}

ErrCode::type CResourceManager::getResource(const InitQubitsReq& req, std::string& resourceid, ResourceData& resourcebytes)
{
    std::map<std::string, ResourceData> resources;
    SINGLETON(CTaskManager)->getAllUseResourceBytes(resources);
    
    return getSignalCpuResource(req, resources, resourceid, resourcebytes);
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
        long long cpumemory = temp.second->m_resource.device.resource.cpu_total_memory;
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

//定时清理资源
void CResourceManager::cleanResource()
{
    std::lock_guard<std::mutex> guard(m_mutex);
    auto iter = m_resourceList.begin();
    while (iter != m_resourceList.end())
    {
        if (iter->second->isTimeout())
        {
            iter->second->cleanResourceOfTask();
            iter = m_resourceList.erase(iter);
        }
        else
        {
            iter++;
        }
    }
}

//清空资源
void CResourceManager::cleanAllResource()
{
    std::lock_guard<std::mutex> guard(m_mutex);
    auto iter = m_resourceList.begin();
    while (iter != m_resourceList.end())
    {
        iter->second->cleanResourceOfTask();
    }
    m_resourceList.clear();
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
        machine.__set_addr(temp.device.machine.addr);
        machine.__set_sys_name(temp.device.machine.sys_name);
        machine.__set_sys_release(temp.device.machine.sys_release);
        machine.__set_sys_version(temp.device.machine.sys_version);
        machine.__set_sys_machine(temp.device.machine.sys_machine);

        ResourceDetail resource;
        resource.__set_cpu_total_memory(temp.device.resource.cpu_total_memory);
        resource.__set_cpu_free_memory(temp.device.resource.cpu_free_memory);

        DeviceDetail device;
        device.__set_machine(machine);
        device.__set_resource(resource);

        devlist.insert(std::pair<std::string, DeviceDetail>(temp.device.machine.addr, device));
    }
}
