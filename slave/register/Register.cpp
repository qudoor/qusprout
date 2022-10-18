#include <sstream>
#include <thread>
#include <chrono>
#include "Register.h"
#include "config/Config.h"
#include "common/qulog.h"
#include "common/Singleton.h"

int getResourceInfo(RpcConnectInfo& rpc, DeviceResourceDetail& device)
{
    //获取系统信息
    MachineSysInfo sys;
    int ret = SINGLETON(CSystemHandle)->getMachineSysInfo(sys);
    if (ret != 0)
    {
        LOG(ERROR) << "getMachineSysInfo failed(ret:" << ret << ").";
        return -1; 
    }

    //获取内存和cpu信息
    MemUseInfo mem;
    ret = SINGLETON(CSystemHandle)->getMemUseInfo(mem);
    if (ret != 0)
    {
        LOG(ERROR) << "getMemUseInfo failed(ret:" << ret << ").";
        return -2;
    }

    rpc.__set_addr(sys.addr);
    rpc.__set_port(SINGLETON(CConfig)->m_listenPort);
    
    MachineInfo machine;
    machine.__set_addr(sys.addr);
    machine.__set_sys_name(sys.sys_name);
    machine.__set_sys_release(sys.sys_release);
    machine.__set_sys_version(sys.sys_version);
    machine.__set_sys_machine(sys.sys_machine);
    device.__set_machine(machine);

    ResourceInfo resource;
    resource.__set_cpu_total_memory(mem.totalRam);
    resource.__set_cpu_free_memory(mem.freeRam);
    device.__set_resource(resource);
    return 0;
}

CRegister::CRegister()
{

}

CRegister::~CRegister()
{
}

//向master注册
int CRegister::registerToMaster()
{
    RpcConnectInfo rpc;
    DeviceResourceDetail device;
    int ret = getResourceInfo(rpc, device);
    if (ret != 0)
    {
        return -1; 
    }
    m_resourceid = device.machine.addr;
    
    //向master注册
    RegisterReq req;
    RegisterResp resp;
    req.__set_resource_id(m_resourceid);
    req.__set_rpc(rpc);
    req.__set_device(device);

    std::lock_guard<std::mutex> guard(m_mutex);
    CMasterClient client;
    ret = client.connect();
    if (ret != 0)
    {
        LOG(ERROR) << "connect master failed(ret:" << ret << ").";
        return -2; 
    }

    client.registerResource(resp, req);
    if (resp.base.code != ErrCode::COM_SUCCESS)
    {
        LOG(ERROR) << "registerResource return error(code:" << resp.base.code << ",msg:" << resp.base.msg << ").";
        return -3; 
    }

    return 0;
}

//处理机器注销接口
int CRegister::unRegisterToMaster()
{
    UnRegisterReq req;
    UnRegisterResp resp;
    req.__set_resource_id(m_resourceid);

    std::lock_guard<std::mutex> guard(m_mutex);
    CMasterClient client;
    int ret = client.connect();
    if (ret != 0)
    {
        LOG(ERROR) << "connect master failed(ret:" << ret << ").";
        return -1; 
    }

    client.unRegister(resp, req);
    if (resp.base.code != ErrCode::COM_SUCCESS)
    {
        LOG(ERROR) << "unRegisterToMaster return error(code:" << resp.base.code << ",msg:" << resp.base.msg << ").";
        return -2; 
    }

    return 0;
}

//处理机器心跳接口
int CRegister::heartbeatToMaster()
{
    if (m_resourceid.empty())
    {
        return registerToMaster();
    }

    RpcConnectInfo rpc;
    DeviceResourceDetail device;
    int ret = getResourceInfo(rpc, device);
    if (ret != 0)
    {
        return -1; 
    }

    HeartbeatReq req;
    HeartbeatResp resp;
    req.__set_resource_id(m_resourceid);
    req.__set_up_resource(true);
    req.__set_device(device);

    {
        std::lock_guard<std::mutex> guard(m_mutex);
        CMasterClient client;
        int ret = client.connect();
        if (ret != 0)
        {
            LOG(ERROR) << "connect master failed(ret:" << ret << ").";
            return -2; 
        }

        client.heartbeat(resp, req);
    }

    if (resp.base.code == ErrCode::type::COM_NOT_REGISTER)
    {
        //未注册，进行注册
        return registerToMaster();
    }
    if (resp.base.code != ErrCode::COM_SUCCESS)
    {
        LOG(ERROR) << "heartbeatToMaster return error(code:" << resp.base.code << ",msg:" << resp.base.msg << ").";
        return -3; 
    }

    return 0;
}
