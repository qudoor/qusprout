#include <sstream>
#include "config/Config.h"
#include "common/qulog.h"
#include "common/Singleton.h"
#include "ResourceManager.h"
#include "interface/ecode_constants.h"
#include "taskmanager/TaskManager.h"

CResourceManager::CResourceManager()
{

}

CResourceManager::~CResourceManager()
{
}

//删除所有机器注册和任务
void CResourceManager::stop()
{
    cleanResource(0);
    LOG(INFO) << "all machine is exited.";
}

//处理机器注册接口
void CResourceManager::registerResource(RegisterResp& resp, const RegisterReq& req)
{
    if (checkRegisterParam(req, resp) == false)
    {
        return;
    }

    time_t now = time(NULL);
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        auto iter  = m_resourceList.find(req.machine.addr);
        if (iter != m_resourceList.end())
        {
            iter->second->updatetime = now;
            iter->second->machine = req.machine;
            iter->second->rpc = req.rpc;
            iter->second->resource = req.resource;
        }
        else
        {
            auto handle = std::make_shared<CResource>();
            handle->createtime = now;
            handle->updatetime = now;
            handle->machine = req.machine;
            handle->rpc = req.rpc;
            handle->resource = req.resource;

            m_resourceList[req.machine.addr] = handle;
        }
    }

    setBase(resp.base, ErrCode::type::COM_SUCCESS);
    return;
}

bool CResourceManager::checkRegisterParam(const RegisterReq& req, RegisterResp& resp)
{
    if (req.machine.addr == "" ||
        req.rpc.addr == "" ||
        req.rpc.port == 0 ||
        req.resource.total_memory == 0)
    {
        setBase(resp.base, ErrCode::type::COM_INVALID_PARAM);
        std::stringstream os("");
        req.printTo(os);
        LOG(ERROR) << "register is invaild praram(" << req << ").";
        return false;
    }

    return true;
}

//处理机器注销接口
void CResourceManager::unRegister(UnRegisterResp& resp, const UnRegisterReq& req)
{
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        auto iter  = m_resourceList.find(req.machine.addr);
        if (iter != m_resourceList.end())
        {
            //清理当前机器资源时，必须释放所有该机器的任务
            SINGLETON(CTaskManager)->cleanTask(iter->first);
            m_resourceList.erase(iter);
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
        auto iter  = m_resourceList.find(req.machine.addr);
        if (iter == m_resourceList.end())
        {
            //机器未注册
            setBase(resp.base, ErrCode::type::QUROOT_NOT_REGISTER);
            std::stringstream os("");
            req.printTo(os);
            LOG(ERROR) << "resource is not register(" << req << ").";
            return;
        }
        //更新updatetime
        iter->second->updatetime = time(NULL);
    }

    setBase(resp.base, ErrCode::type::COM_SUCCESS);
    return;
}

//处理上报资源接口
void CResourceManager::reportResource(ReportResourceResp& resp, const ReportResourceReq& req)
{
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        auto iter  = m_resourceList.find(req.machine.addr);
        if (iter == m_resourceList.end())
        {
            //机器未注册
            setBase(resp.base, ErrCode::type::QUROOT_NOT_REGISTER);
            std::stringstream os("");
            req.printTo(os);
            LOG(ERROR) << "resource is not register(" << req << ").";
            return;
        }
        //更新updatetime
        iter->second->updatetime = time(NULL);
        iter->second->resource = req.resource;
    }

    setBase(resp.base, ErrCode::type::COM_SUCCESS);
    return;
}

//上报统计信息
void CResourceManager::ReportStatisticsInfo(ReportStatisticsInfoResp& resp, const ReportStatisticsInfoReq& req)
{
    {
        std::lock_guard<std::mutex> guard(m_mutexSis);
        auto& temp = m_sisList[req.machine.addr];
        temp.__set_fixed_info(req.sis.fixed_info);
        temp.__set_dyna_info(req.sis.dyna_info);
        temp.__set_curr_task_num(req.sis.curr_task_num);
        if (temp.begin_time == 0)
        {
            temp.__set_begin_time(req.sis.begin_time);
        }
        temp.__set_end_time(req.sis.end_time);

        auto alliter = req.sis.all_task_num.begin();
        for (; alliter != req.sis.all_task_num.end(); ++alliter)
        {
            temp.all_task_num[alliter->first].__set_tag(alliter->second.tag);
            int num = alliter->second.num + temp.all_task_num[alliter->first].num;
            temp.all_task_num[alliter->first].__set_num(num);
        }

        auto countiter = req.sis.inter_cout.begin();
        for (; countiter != req.sis.inter_cout.end(); ++countiter)
        {
            temp.inter_cout[countiter->first].__set_tag(countiter->second.tag);
            int count = countiter->second.count + temp.inter_cout[countiter->first].count;
            temp.inter_cout[countiter->first].__set_count(count);
        }

        auto timeiter = req.sis.inter_time.begin();
        for (; timeiter != req.sis.inter_time.end(); ++timeiter)
        {
            temp.inter_time[timeiter->first].__set_tag(timeiter->second.tag);
            temp.inter_time[timeiter->first].__set_elapsed(timeiter->second.elapsed);
            int count = timeiter->second.count + temp.inter_time[timeiter->first].count;
            temp.inter_time[timeiter->first].__set_count(count);
        }
 
        auto codeiter = req.sis.code_count.begin();
        for (; codeiter != req.sis.code_count.end(); ++codeiter)
        {
            temp.code_count[codeiter->first].__set_tag(codeiter->second.tag);
            temp.code_count[codeiter->first].__set_code(codeiter->second.code);
            int count = codeiter->second.count + temp.code_count[codeiter->first].count;
            temp.code_count[codeiter->first].__set_count(count);
        }
    }

    setBase(resp.base, ErrCode::type::COM_SUCCESS);
    return;
}

//获取统计信息
void CResourceManager::GetStatisticsInfo(GetStatisticsInfoResp& resp, const GetStatisticsInfoReq& req)
{
    std::map<std::string, StatisticsInfo> sislist;
    {
        std::lock_guard<std::mutex> guard(m_mutexSis);
        sislist.insert(m_sisList.begin(), m_sisList.end());
        m_sisList.clear();
    }
    resp.__set_sis_list(sislist);

    setBase(resp.base, ErrCode::type::COM_SUCCESS);
    return;
}

void CResourceManager::setBase(BaseCode& base, const ErrCode::type& code)
{
    base.__set_code(code);
    auto ptr = g_ecode_constants.ErrMsg.find(code);
    base.__set_msg(ptr->second);
}

//检查cpu资源是否足够
ErrCode::type CResourceManager::checkCpuResource(const InitQubitsReq& req, std::string& addr, std::string& rpcAddr, int& rpcPort, std::vector<std::string>& hosts)
{
    std::lock_guard<std::mutex> guard(m_mutex);
    size_t ressize = m_resourceList.size();
    if (ressize == 0)
    {
        //无可用资源
        LOG(ERROR) << "no resource is available(taskId:" << req.id << ").";
        return ErrCode::type::QUROOT_NOT_RESOURCE;
    }

    long long needmemory = 0;
    unsigned int index = m_indexRand++;
    size_t currindex = index % ressize;
    std::shared_ptr<CResource> handle = nullptr;
    //判断当前可用内存是否足够
    if (req.exec_type == ExecCmdType::type::ExecTypeCpuMpi)
    {
        int numranks = getNumRanks(req.qubits, ressize);
        needmemory = calcQubitBytes(req, numranks);

        size_t i = 0;
        auto iter = m_resourceList.begin();
        for (; iter != m_resourceList.end(); ++iter)
        {
           if (needmemory <= 0 || iter->second->resource.free_memory < needmemory)
            {
                //内存不足
                LOG(ERROR) << "memory resource is enough(taskId:" << req.id << ",free_memory:" << iter->second->resource.free_memory << ",needmemory:" << needmemory << ").";
                return ErrCode::type::QUROOT_MEM_NOT_ENOUGH;
            } 
            if (currindex == i)
            {
                handle = iter->second;
                hosts.insert(hosts.begin(), iter->second->rpc.addr);
            }
            else
            {
                hosts.push_back(iter->second->rpc.addr);
            }
            ++i;
        }
    }
    else
    {
        needmemory = calcQubitBytes(req, 1);

        auto handlebegin = m_resourceList.begin();
        std::advance(handlebegin, currindex);
        handle = handlebegin->second;

        if (needmemory <= 0 || handle->resource.free_memory < needmemory)
        {
            //内存不足
            LOG(ERROR) << "memory resource is enough(taskId:" << req.id << ",free_memory:" << handle->resource.free_memory << ",needmemory:" << needmemory << ").";
            return ErrCode::type::QUROOT_MEM_NOT_ENOUGH;
        }
    }

    addr = handle->machine.addr;
    rpcAddr = handle->rpc.addr;
    rpcPort = handle->rpc.port;
    LOG(INFO) << "applyResource(needmemory:" << needmemory << ",free_memory:" << handle->resource.free_memory << ",addr:" << addr << ")";
    return ErrCode::type::COM_SUCCESS;
}

//检查gpu资源是否足够
ErrCode::type CResourceManager::checkGpuResource(const InitQubitsReq& req, std::string& addr, std::string& rpcAddr, int& rpcPort, std::vector<std::string>& hosts)
{
    //1.计算内存
    long long needmemory = 0;
    needmemory = calcQubitBytes(req, 1);

    //2.检查cpu内存
    std::lock_guard<std::mutex> guard(m_mutex);
    size_t ressize = m_resourceList.size();
    if (ressize == 0)
    {
        //无可用资源
        LOG(ERROR) << "no resource is available(taskId:" << req.id << ").";
        return ErrCode::type::QUROOT_NOT_RESOURCE;
    }

    unsigned int index = m_indexRand++;
    size_t currindex = index % ressize;
    std::shared_ptr<CResource> handle = nullptr;

    auto handlebegin = m_resourceList.begin();
    std::advance(handlebegin, currindex);
    handle = handlebegin->second;

    if (needmemory <= 0 || handle->resource.free_memory < needmemory)
    {
        //内存不足
        LOG(ERROR) << "memory resource is enough(taskId:" << req.id << ",free_memory:" << handle->resource.free_memory << ",needmemory:" << needmemory << ").";
        return ErrCode::type::QUROOT_MEM_NOT_ENOUGH;
    }

    addr = handle->machine.addr;
    rpcAddr = handle->rpc.addr;
    rpcPort = handle->rpc.port;
    LOG(INFO) << "applyResource(needmemory:" << needmemory << ",free_memory:" << handle->resource.free_memory << ",addr:" << addr << ")";
    
    //3.检查gpu内存
    
    return ErrCode::type::COM_SUCCESS;
}

//检查用户自定义资源
ErrCode::type CResourceManager::checkFixedResource(const InitQubitsReq& req, const std::vector<std::string>& hosts, std::string& addr, std::string& rpcAddr, int& rpcPort)
{
    long long needmemory = calcQubitBytes(req, 1);

    std::lock_guard<std::mutex> guard(m_mutex);
    size_t ressize = m_resourceList.size();
    if (ressize == 0)
    {
        //无可用资源
        LOG(ERROR) << "no resource is available(taskId:" << req.id << ").";
        return ErrCode::type::QUROOT_NOT_RESOURCE;
    }

    auto iter = m_resourceList.find(hosts[0]);
    if (iter == m_resourceList.end())
    {
        LOG(ERROR) << "resource is not exist(addr:" << hosts[0] << ").";
        return ErrCode::type::COM_INVALID_PARAM;
    }

    addr = iter->second->machine.addr;
    rpcAddr = iter->second->rpc.addr;
    rpcPort = iter->second->rpc.port;
    LOG(INFO) << "applyResource(needmemory:" << needmemory << ",free_memory:" << iter->second->resource.free_memory << ",addr:" << addr << ")";

    return ErrCode::type::COM_SUCCESS;
}

//定时清理资源
void CResourceManager::timerCleanResource()
{
    cleanResource(SINGLETON(CConfig)->m_resourceTimeOutDuration);
}

//清理资源
void CResourceManager::cleanResource(const int timeOutDuration)
{
    std::ostringstream os("");
    time_t now = time(NULL);

    std::lock_guard<std::mutex> guard(m_mutex);
    auto iter = m_resourceList.begin();
    while (iter != m_resourceList.end())
    {
        if (now - iter->second->updatetime >= timeOutDuration)
        {
            LOG(WARNING) << "clear resource(resourceid:" << iter->first << ",updatetime:" << iter->second->updatetime << ",timeOutDuration:" << timeOutDuration << ").";
            //清理当前机器资源时，必须释放所有该机器的任务
            SINGLETON(CTaskManager)->cleanTask(iter->first);
            iter = m_resourceList.erase(iter);
        }
        else
        {
            os << iter->first << ";";
            iter++;
        }
    }

    LOG(INFO) << "curr resource list(" << os.str() << ").";
}

long long CResourceManager::calcQubitBytes(const InitQubitsReq& req, const int numranks)
{
    if (req.qubits <= 0)
    {
        return 0;
    }

    long long havebytes = 0;
    if (req.density)
    {
        if (req.exec_type == ExecCmdType::type::ExecTypeCpuMpi)
            havebytes = 32*1024*pow(2, req.qubits)/numranks;
        else
            havebytes = 16*1024*pow(2, req.qubits);
    }
    else
    {  
        if (req.exec_type == ExecCmdType::type::ExecTypeCpuMpi)
            havebytes = 32*pow(2, req.qubits)/numranks;
        else
            havebytes = 16*pow(2, req.qubits);
    }
    return havebytes;
}

//获取启动mpi的进程数
int CResourceManager::getNumRanks(const int numQubits, const int hostSize)
{
    //只有一台机器就只开一个进程
    if (1 == hostSize)
    {
        return 1;
    }

    //取小于等于并且离hostSize最近的2的n次方
    long unsigned int numranks = 1;
    long unsigned int newnumranks = 1;
    while(newnumranks <= (long unsigned int)hostSize)
    {
        numranks = newnumranks;
        newnumranks *= 2;
    }
    
    //qubit太小的时候取2的numQubits次方作为进程数
    long unsigned int numAmps = (1UL<<numQubits);
    if (numAmps < numranks)
    {
        numranks = numAmps;
    }

    return (int)numranks;
}

//获取所有的资源
void CResourceManager::getResource(std::map<std::string, DeviceDetail>& devlist)
{
    std::lock_guard<std::mutex> guard(m_mutex);
    auto iter = m_resourceList.begin();
    for (; iter != m_resourceList.end(); ++iter)
    {
        auto temp = iter->second;

        MachineDetail machine;
        machine.__set_addr(temp->machine.addr);
        machine.__set_sys_name(temp->machine.sys_name);
        machine.__set_sys_release(temp->machine.sys_release);
        machine.__set_sys_version(temp->machine.sys_version);
        machine.__set_sys_machine(temp->machine.sys_machine);

        ResourceDetail resource;
        resource.__set_cpu_total_memory(temp->resource.total_memory);
        resource.__set_cpu_free_memory(temp->resource.free_memory);

        DeviceDetail device;
        device.__set_machine(machine);
        device.__set_resource(resource);

        devlist.insert(std::pair<std::string, DeviceDetail>(temp->machine.addr, device));
    }
}
