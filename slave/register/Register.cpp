#include <sstream>
#include <thread>
#include <chrono>
#include "Register.h"
#include "config/Config.h"
#include "common/qulog.h"
#include "common/Singleton.h"
#include "statistics/Statistics.h"
#include "interface/statistics_types.h"

CRegister::CRegister()
{

}

CRegister::~CRegister()
{
}

//初始化
int CRegister::init(const int timeout)
{
    //获取系统信息
    MachineSysInfo sys;
    int ret = SINGLETON(CSystemHandle)->getMachineSysInfo(sys);
    if (ret != 0)
    {
        LOG(ERROR) << "getMachineSysInfo failed(ret:" << ret << ").";
        return -1; 
    }
    //缓存机器信息
    m_machine = sys;
    LOG(INFO) << "CRegister init(addr:" << m_machine.addr << ",sys_name:" << m_machine.sys_name 
        << ",sys_release:" << m_machine.sys_release << ",sys_version:" << m_machine.sys_version 
        << ",sys_machine:" << m_machine.sys_machine << ",create_time:" << m_machine.create_time << ").";
      
    std::lock_guard<std::mutex> guard(m_mutex);
    m_client = std::make_shared<CMasterClient>();
    int count = 500;
    if (timeout > 0)
    {
        count = timeout/100;
    }
    while(count > 0)
    {
        ret = m_client->init(SINGLETON(CConfig)->m_masterAddr, SINGLETON(CConfig)->m_masterPort);
        if (0 == ret)
        {
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        --count;
    }
    if (ret != 0)
    {
        LOG(ERROR) << "init CMasterClient failed(ret:" << ret 
            << ",masterAddr:" << SINGLETON(CConfig)->m_masterAddr 
            << ",masterPort:" << SINGLETON(CConfig)->m_masterPort 
            << ").";
        return -2;
    }

    return 0;
}

//停止注册
void CRegister::stop()
{
    LOG(INFO) << "begin curr machine is exited(addr:" << m_machine.addr << ").";
    unRegisterToMaster();
    LOG(INFO) << "curr machine is exited(addr:" << m_machine.addr << ").";
}

//向master注册
int CRegister::registerToMaster()
{
    //获取内存和cpu信息
    MemUseInfo mem;
    int ret = SINGLETON(CSystemHandle)->getMemUseInfo(mem);
    if (ret != 0)
    {
        LOG(ERROR) << "getMemUseInfo failed(ret:" << ret << ").";
        return -1;
    }

    //向master注册
    RegisterReq req;
    RegisterResp resp;
    req.__set_type(RegType::RegType_Cpu_Simulator);
    req.__set_seq(getSeq(0));

    RpcConnectInfo rpc;
    rpc.__set_addr(m_machine.addr);
    rpc.__set_port(SINGLETON(CConfig)->m_listenPort);
    req.__set_rpc(rpc);
    
    MachineInfo machine;
    machine.__set_addr(m_machine.addr);
    machine.__set_sys_name(m_machine.sys_name);
    machine.__set_sys_release(m_machine.sys_release);
    machine.__set_sys_version(m_machine.sys_version);
    machine.__set_sys_machine(m_machine.sys_machine);
    machine.__set_create_time(time(NULL));
    req.__set_machine(machine);

    ResourceInfo resource;
    resource.__set_total_cpu(0);
    resource.__set_free_cpu(0);
    resource.__set_total_memory(mem.totalRam);
    resource.__set_free_memory(mem.freeRam);
    resource.__set_create_time(mem.currTime);
    req.__set_resource(resource);

    std::lock_guard<std::mutex> guard(m_mutex);
    m_client->registerResource(resp, req);
    if (resp.base.code != ErrCode::COM_SUCCESS)
    {
        LOG(ERROR) << "registerResource return error(code:" << resp.base.code << ",msg:" << resp.base.msg << ").";
        return -2; 
    }
    m_isreg = true;

    return 0;
}

//处理机器注销接口
int CRegister::unRegisterToMaster()
{
    if (false == m_isreg)
    {
        LOG(ERROR) << "not need unRegisterToMaster.";
        return 0;
    }

    UnRegisterReq req;
    UnRegisterResp resp;
    req.__set_type(RegType::RegType_Cpu_Simulator);
    req.__set_seq(getSeq(1));

    MachineInfo machine;
    machine.__set_addr(m_machine.addr);
    machine.__set_sys_name(m_machine.sys_name);
    machine.__set_sys_release(m_machine.sys_release);
    machine.__set_sys_version(m_machine.sys_version);
    machine.__set_sys_machine(m_machine.sys_machine);
    machine.__set_create_time(time(NULL));
    req.__set_machine(machine);

    std::lock_guard<std::mutex> guard(m_mutex);
    if (true == m_isreg)
    {
        m_client->unRegister(resp, req);
        if (resp.base.code != ErrCode::COM_SUCCESS)
        {
            LOG(ERROR) << "unRegister return error(code:" << resp.base.code << ",msg:" << resp.base.msg << ").";
            return -2; 
        }
        m_isreg = false;
    }
    
    return 0;
}

//处理机器心跳接口
int CRegister::heartbeatToMaster()
{
    HeartbeatReq req;
    HeartbeatResp resp;
    req.__set_type(RegType::RegType_Cpu_Simulator);
    req.__set_seq(getSeq(2));

    MachineInfo machine;
    machine.__set_addr(m_machine.addr);
    machine.__set_sys_name(m_machine.sys_name);
    machine.__set_sys_release(m_machine.sys_release);
    machine.__set_sys_version(m_machine.sys_version);
    machine.__set_sys_machine(m_machine.sys_machine);
    machine.__set_create_time(time(NULL));
    req.__set_machine(machine);

    {
        std::lock_guard<std::mutex> guard(m_mutex);
        m_client->heartbeat(resp, req);
    }
    if (resp.base.code == ErrCode::QUROOT_NOT_REGISTER)
    {
        //未注册，进行注册
        registerToMaster();
    }
    else if (resp.base.code != ErrCode::COM_SUCCESS)
    {
        LOG(ERROR) << "heartbeat return error(code:" << resp.base.code << ",msg:" << resp.base.msg << ").";
        return -2; 
    }

    return 0;
}

//处理上报资源接口
int CRegister::reportResourceToMaster()
{
    //获取内存和cpu信息
    MemUseInfo mem;
    int ret = SINGLETON(CSystemHandle)->getMemUseInfo(mem);
    if (ret != 0)
    {
        LOG(INFO) << "getMemUseInfo failed(ret:" << ret << ").";
        return -1;
    }

    ReportResourceReq req;
    ReportResourceResp resp;
    req.__set_type(RegType::RegType_Cpu_Simulator);
    req.__set_seq(getSeq(3));

    MachineInfo machine;
    machine.__set_addr(m_machine.addr);
    machine.__set_sys_name(m_machine.sys_name);
    machine.__set_sys_release(m_machine.sys_release);
    machine.__set_sys_version(m_machine.sys_version);
    machine.__set_sys_machine(m_machine.sys_machine);
    machine.__set_create_time(time(NULL));
    req.__set_machine(machine);
    
    ResourceInfo resource;
    resource.__set_total_cpu(0);
    resource.__set_free_cpu(0);
    resource.__set_total_memory(mem.totalRam);
    resource.__set_free_memory(mem.freeRam);
    resource.__set_create_time(mem.currTime);
    req.__set_resource(resource);

    {
        std::lock_guard<std::mutex> guard(m_mutex);
        m_client->reportResource(resp, req);
    }
    if (resp.base.code == ErrCode::QUROOT_NOT_REGISTER)
    {
        //未注册，进行注册
        registerToMaster();
    }
    else if (resp.base.code != ErrCode::COM_SUCCESS)
    {
        LOG(ERROR) << "reportResource return error(req:" << req.seq << ",code:" << resp.base.code << ",msg:" << resp.base.msg << ").";
        return -3; 
    }

    time_t now = time(NULL);
    if (now - m_lastPrintTime >= 1800)
    {
        LOG(INFO) << "report resource to master(req:" << getPrint<ReportResourceReq>(req) << ",resp:" << getPrint<ReportResourceResp>(resp) << ").";
        m_lastPrintTime = now;
    }

    return 0;
}

//上报统计信息
void CRegister::ReportStatisticsInfo()
{
    ReportStatisticsInfoReq req;
    ReportStatisticsInfoResp resp;

    StatisticsInfo sis;
    SINGLETON(CStatistics)->getStatisticsInfo(sis);
    req.__set_seq(getSeq(4));
    req.__set_sis(sis);

    MachineInfo machine;
    machine.__set_addr(m_machine.addr);
    machine.__set_sys_name(m_machine.sys_name);
    machine.__set_sys_release(m_machine.sys_release);
    machine.__set_sys_version(m_machine.sys_version);
    machine.__set_sys_machine(m_machine.sys_machine);
    machine.__set_create_time(time(NULL));
    req.__set_machine(machine);

    std::lock_guard<std::mutex> guard(m_mutex);
    m_client->ReportStatisticsInfo(resp, req);
}

//获取流水
std::string CRegister::getSeq(const int type)
{
    int seq = m_seqIndex++;
    std::ostringstream os("");
    os << m_machine.addr << type << time(NULL) << seq;
    return os.str();
}

//获取hostname
std::string CRegister::getHostName()
{
    if (m_machine.addr != "")
    {
       return m_machine.addr;
    }

    MachineSysInfo sys;
    int ret = SINGLETON(CSystemHandle)->getMachineSysInfo(sys);
    if (ret != 0)
    {
        LOG(ERROR) << "getMachineSysInfo failed(ret:" << ret << ").";
        return ""; 
    }

     return sys.addr;
}
