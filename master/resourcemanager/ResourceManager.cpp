#include <sstream>
#include "config/Config.h"
#include "common/qulog.h"
#include "common/Singleton.h"
#include "ResourceManager.h"
#include "interface/ecode_constants.h"
#include "taskmanager/TaskManager.h"

CResource::CResource(const RegisterReq& req)
{
    m_createtime = getCurrMs();
    m_updatetime = m_createtime;
    m_resource = req;
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

//处理机器注册接口
void CResourceManager::registerResource(RegisterResp& resp, const RegisterReq& req)
{
    if (req.resource_id == "" ||
        req.rpc.addr == "" ||
        req.rpc.port == 0 ||
        req.device.resource.cpu_total_memory == 0)
    {
        setBase(resp.base, ErrCode::type::COM_INVALID_PARAM);
        LOG(ERROR) << "register is invaild praram(" << getPrint(req) << ").";
        return;
    }

    {
        std::lock_guard<std::mutex> guard(m_mutex);
        auto iter  = m_resourceList.find(req.resource_id);
        if (iter != m_resourceList.end())
        {
            iter->second->updateResource(req);
        }
        else
        {
            auto handle = std::make_shared<CResource>(req);
            m_resourceList[req.resource_id] = handle;
        }
    }

    setBase(resp.base, ErrCode::type::COM_SUCCESS);
}

//处理机器注销接口
void CResourceManager::unRegister(UnRegisterResp& resp, const UnRegisterReq& req)
{
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        auto iter  = m_resourceList.find(req.resource_id);
        if (iter != m_resourceList.end())
        {
            //清理当前机器资源时，必须释放所有该机器的任务
            iter->second->cleanResourceOfTask();
            m_resourceList.erase(iter);
            LOG(INFO) << "unRegister(" << getPrint(req) << ").";
        }
    }

    setBase(resp.base, ErrCode::type::COM_SUCCESS);
    return;
}

//处理机器心跳接口
void CResourceManager::heartbeat(HeartbeatResp& resp, const HeartbeatReq& req)
{
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        auto iter  = m_resourceList.find(req.resource_id);
        if (iter == m_resourceList.end())
        {
            //机器未注册
            setBase(resp.base, ErrCode::type::QUROOT_NOT_REGISTER);
            LOG(ERROR) << "resource is not register(" << getPrint(req) << ").";
            return;
        }
        iter->second->updateResource(req);
    }

    setBase(resp.base, ErrCode::type::COM_SUCCESS);
    return;
}

ErrCode::type CResourceManager::getResource(const InitQubitsReq& req, std::string& resourceid, RpcConnectInfo& addr, std::vector<std::string>& hosts, ResourceData& resourcebytes)
{
    std::map<std::string, ResourceData> resources;
    SINGLETON(CTaskManager)->getAllUseResourceBytes(resources);

    if (ExecCmdType::type::ExecTypeGpuSingle == req.exec_type)
    {
        return getSignalGpuResource(req, resources, resourceid, addr, hosts, resourcebytes);
    }

    if (ExecCmdType::type::ExecTypeCpuMpi == req.exec_type)
    {
        return getMpiCpuResource(req, resources, resourceid, addr, hosts, resourcebytes);
    }
    
    return getSignalCpuResource(req, resources, resourceid, addr, hosts, resourcebytes);
}

ErrCode::type CResourceManager::getSignalGpuResource(const InitQubitsReq& req, const std::map<std::string, ResourceData>& useresource, std::string& resourceid, RpcConnectInfo& addr, std::vector<std::string>& hosts, ResourceData& resourcebytes)
{
    std::lock_guard<std::mutex> guard(m_mutex);
    size_t ressize = m_resourceList.size();
    if (ressize == 0)
    {
        //无可用资源
        LOG(ERROR) << "no resource is available(taskId:" << req.id << ").";
        return ErrCode::type::QUROOT_NOT_RESOURCE;
    }

    long long int singlebytes = calcSingleBytes(req.qubits, req.density);
    std::vector<std::string> cpuhosts;
    if (req.hosts.size() > 0)
    {
        auto iter = m_resourceList.find(req.hosts[0]);
        if (iter != m_resourceList.end())
        {
            long long cpumemory = iter->second->m_resource.device.resource.cpu_total_memory;
            long long gpumemory = iter->second->m_resource.device.resource.gpu_total_memory;
            auto addr = iter->second->getAddr();
            auto finditer = useresource.find(addr);
            if (finditer != useresource.end())
            {
                cpumemory -= finditer->second.cpubytes;
                gpumemory -= finditer->second.gpubytes;
            }
            if (cpumemory >= singlebytes && gpumemory >= singlebytes)
            {
                cpuhosts.push_back(addr);
            }
        }
    }
    else
    {
        for (auto& temp : m_resourceList)
        {
            long long cpumemory = temp.second->m_resource.device.resource.cpu_total_memory;
            long long gpumemory = temp.second->m_resource.device.resource.gpu_total_memory;
            auto addr = temp.second->getAddr();
            auto finditer = useresource.find(addr);
            if (finditer != useresource.end())
            {
                cpumemory -= finditer->second.cpubytes;
                gpumemory -= finditer->second.gpubytes;
            }
            if (cpumemory >= singlebytes && gpumemory >= singlebytes)
            {
                cpuhosts.push_back(addr);
            }
        }
    }

    size_t cpusize = cpuhosts.size();
    if (0 == cpusize)
    {
        LOG(ERROR) << "resource memory is not enough(taskId:" << req.id << ").";
        return ErrCode::type::QUROOT_MEM_NOT_ENOUGH;
    }

    resourcebytes.cpubytes = singlebytes;
    resourcebytes.gpubytes = 0;
    auto index = m_index++%cpusize;
    auto host = cpuhosts[index];
    resourceid = m_resourceList[host]->m_resource.resource_id;
    m_resourceList[host]->getAddr(addr);
    hosts.push_back(host);
    return ErrCode::type::COM_SUCCESS;
}

ErrCode::type CResourceManager::getMpiCpuResource(const InitQubitsReq& req, const std::map<std::string, ResourceData>& useresource, std::string& resourceid, RpcConnectInfo& addr, std::vector<std::string>& hosts, ResourceData& resourcebytes)
{
    std::lock_guard<std::mutex> guard(m_mutex);
    size_t ressize = m_resourceList.size();
    if (ressize == 0)
    {
        //无可用资源
        LOG(ERROR) << "no resource is available(taskId:" << req.id << ").";
        return ErrCode::type::QUROOT_NOT_RESOURCE;
    }

    int numranks = 0;
    long long int mpibytes = 0;
    std::vector<std::string> mpicpuhosts;
    if (req.hosts.size() > 0)
    {
        numranks = req.hosts.size();
        mpibytes = calcMpiBytes(req.qubits, req.density, numranks);
        for (auto& temp : req.hosts)
        {
            auto iter = m_resourceList.find(temp);
            if (iter != m_resourceList.end())
            {
                long long cpumemory = iter->second->m_resource.device.resource.cpu_total_memory;
                auto addr = iter->second->getAddr();
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
        }
    }
    else
    {
        numranks = getNumRanks(req.qubits);
        mpibytes = calcMpiBytes(req.qubits, req.density, numranks);
        for (auto& temp : m_resourceList)
        {
            long long cpumemory = temp.second->m_resource.device.resource.cpu_total_memory;
            auto addr = temp.second->getAddr();
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
    }

    size_t mpisize = mpicpuhosts.size();
    if (0 == mpisize && mpisize < (size_t)numranks)
    {
        LOG(ERROR) << "resource memory is not enough(taskId:" << req.id << ").";
        return ErrCode::type::QUROOT_MEM_NOT_ENOUGH;
    }

    resourcebytes.cpubytes = mpibytes;
    resourcebytes.gpubytes = 0;
    int index = m_index++%mpisize;
    auto host = mpicpuhosts[index];
    resourceid = m_resourceList[host]->m_resource.resource_id;
    m_resourceList[host]->getAddr(addr);
    for (int i = 0; i < numranks; ++i)
    {
        int j = index++%mpisize;
        hosts.push_back(mpicpuhosts[j]);
    }
    return ErrCode::type::COM_SUCCESS;
}

ErrCode::type CResourceManager::getSignalCpuResource(const InitQubitsReq& req, const std::map<std::string, ResourceData>& useresource, std::string& resourceid, RpcConnectInfo& addr, std::vector<std::string>& hosts, ResourceData& resourcebytes)
{
    std::lock_guard<std::mutex> guard(m_mutex);
    size_t ressize = m_resourceList.size();
    if (ressize == 0)
    {
        //无可用资源
        LOG(ERROR) << "no resource is available(taskId:" << req.id << ").";
        return ErrCode::type::QUROOT_NOT_RESOURCE;
    }

    long long int singlebytes = calcSingleBytes(req.qubits, req.density);
    std::vector<std::string> cpuhosts;
    if (req.hosts.size() > 0)
    {
        auto iter = m_resourceList.find(req.hosts[0]);
        if (iter != m_resourceList.end())
        {
            long long cpumemory = iter->second->m_resource.device.resource.cpu_total_memory;
            auto addr = iter->second->getAddr();
            auto finditer = useresource.find(addr);
            if (finditer != useresource.end())
            {
                cpumemory -= finditer->second.cpubytes;
            }
            LOG(INFO) << "---------getSignalCpuResource self---------------(total memory:" << iter->second->m_resource.device.resource.cpu_total_memory << ",left memory:" << cpumemory << ",need memory:" << singlebytes << ").";
            if (cpumemory >= singlebytes)
            {
                cpuhosts.push_back(addr);
            }
        }
    }
    else
    {
        for (auto& temp : m_resourceList)
        {
            long long cpumemory = temp.second->m_resource.device.resource.cpu_total_memory;
            auto addr = temp.second->getAddr();
            auto finditer = useresource.find(addr);
            if (finditer != useresource.end())
            {
                cpumemory -= finditer->second.cpubytes;
            }
            LOG(INFO) << "---------getSignalCpuResource---------------(total memory:" << temp.second->m_resource.device.resource.cpu_total_memory << ",left memory:" << cpumemory << ",need memory:" << singlebytes << ").";
            if (cpumemory >= singlebytes)
            {
                cpuhosts.push_back(addr);
            }
        }
    }

    size_t cpusize = cpuhosts.size();
    if (0 == cpusize)
    {
        LOG(ERROR) << "resource memory is not enough(taskId:" << req.id << ").";
        return ErrCode::type::QUROOT_MEM_NOT_ENOUGH;
    }

    resourcebytes.cpubytes = singlebytes;
    resourcebytes.gpubytes = 0;
    auto index = m_index++%cpusize;
    auto host = cpuhosts[index];
    resourceid = m_resourceList[host]->m_resource.resource_id;
    m_resourceList[host]->getAddr(addr);
    hosts.push_back(host);
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
