/****************************************************************************
*
* This code is licensed under the Apache License, Version 2.0. You may
* obtain a copy of this license in the LICENSE.txt file in the root directory
* of this source tree or at http://www.apache.org/licenses/LICENSE-2.0.
*
* Any modifications or derivative works of this code must retain this
* copyright notice, and modified files need to carry a notice indicating
* that they have been altered from the originals.
*
****************************************************************************/

#include <sstream>
#include <thread>
#include <chrono>
#include "Register.h"
#include "config/Config.h"
#include "common/qulog.h"
#include "common/Singleton.h"
#include "resourcemanager/ResourceManager.h"

CRegister::CRegister()
{

}

CRegister::~CRegister()
{
}

//向quroot注册
int CRegister::resourceRegister()
{
    //获取系统信息
    MachineSysInfo sys;
    int ret = SINGLETON(CSystemHandle)->getMachineSysInfo(sys);
    if (ret != 0)
    {
        LOG(ERROR) << "getMachineSysInfo failed(ret:" << ret << ").";
        return -1; 
    }
    m_resourceid = sys.addr;

    //向quroot注册
    ResourceRegisterReq req;
    ResourceRegisterResp resp;
    req.__set_resource_id(m_resourceid);
    req.__set_name("simulator");
    req.__set_protocol(0);
    
    TcpResourceDetail tpcresource;
    AddrDetail addr;
    addr.__set_addr(sys.addr);
    if (SINGLETON(CConfig)->m_listenAddr != "")
    {
        addr.__set_addr(SINGLETON(CConfig)->m_listenAddr);
    }
    addr.__set_port(SINGLETON(CConfig)->m_listenPort);
    tpcresource.__set_addr_detail(addr);
    std::map<std::string, DeviceDetail> devlist;
    SINGLETON(CResourceManager)->getAllResource(devlist);
    tpcresource.__set_dev_list(devlist);
    req.__set_tcp_resource(tpcresource);

    DeviceCapacity devcap;
    std::vector<GateCapacity> gatecaplist;
    auto iter = SINGLETON(CConfig)->m_supportGate.begin();
    for (; iter != SINGLETON(CConfig)->m_supportGate.end(); ++iter)
    {
        GateCapacity cap;
        cap.__set_device(iter->first);

        std::vector<std::string> gates;
        for (auto gate : iter->second)
        {
            gates.push_back(gate);
        }
        cap.__set_gates(gates);
        gatecaplist.push_back(cap);
    }
    devcap.__set_gate_cap_list(gatecaplist);
    req.__set_cap(devcap);

    std::lock_guard<std::mutex> guard(m_mutex);
    CQurootClient client;
    ret = client.connect();
    if (ret != 0)
    {
        LOG(ERROR) << "connect quroot failed(ret:" << ret << ").";
        return -2; 
    }

    client.resourceRegister(resp, req);
    if (resp.base.code != ErrCode::type::COM_SUCCESS)
    {
        LOG(ERROR) << "resourceRegister return error(code:" << resp.base.code << ",msg:" << resp.base.msg << ").";
        return -3; 
    }

    return 0;
}

//处理机器注销接口
int CRegister::unResourceRegister()
{
    UnResourceRegisterReq req;
    UnResourceRegisterResp resp;
    req.__set_resource_id(m_resourceid);

    std::lock_guard<std::mutex> guard(m_mutex);
    CQurootClient client;
    int ret = client.connect();
    if (ret != 0)
    {
        LOG(ERROR) << "connect quroot failed(ret:" << ret << ").";
        return -1; 
    }

    client.unResourceRegister(resp, req);
    if (resp.base.code != ErrCode::type::COM_SUCCESS)
    {
        LOG(ERROR) << "unResourceRegister return error(code:" << resp.base.code << ",msg:" << resp.base.msg << ").";
        return -2; 
    }

    return 0;
}

//处理机器心跳接口
int CRegister::heart()
{
    if (m_resourceid.empty())
    {
        return resourceRegister();
    }

    HeartReq req;
    HeartResp resp;
    req.__set_resource_id(m_resourceid);

    std::map<std::string, DeviceDetail> devlist;
    SINGLETON(CResourceManager)->getAllResource(devlist);
    req.__set_up_resource(true);
    req.__set_dev_list(devlist);

    {
        std::lock_guard<std::mutex> guard(m_mutex);
        CQurootClient client;
        int ret = client.connect();
        if (ret != 0)
        {
            LOG(ERROR) << "connect quroot failed(ret:" << ret << ").";
            return -1; 
        }

        client.heart(resp, req);
    }

    if (resp.base.code == ErrCode::type::COM_NOT_REGISTER)
    {
        //未注册，进行注册
        return resourceRegister();
    }
    if (resp.base.code != ErrCode::COM_SUCCESS)
    {
        LOG(ERROR) << "heart return error(code:" << resp.base.code << ",msg:" << resp.base.msg << ").";
        return -2; 
    }

    return 0;
}
