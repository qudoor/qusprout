#ifndef __H__RESOURCEMANAGER__H__
#define __H__RESOURCEMANAGER__H__

#include <map>
#include <mutex>
#include <memory>
#include <string>
#include <atomic>
#include "comm/SelfStruct.h"
#include "interface/ecode_types.h"
#include "interface/resource_types.h"
#include "interface/qusproutdata_types.h"
#include "interface/statistics_types.h"
#include "interface/qurootresource_types.h"

//机器资源
struct CResource
{
    //创建时间
    long createtime{0};

    //更新时间
    long updatetime{0};

    //机器信息
    MachineInfo machine;

    //rpc连接信息
    RpcConnectInfo rpc;

    //机器资源信息
    ResourceInfo resource;
};

class CResourceManager
{
public:
    CResourceManager();
    ~CResourceManager();

    //删除所有机器注册和任务
    void stop();
    
    //处理机器注册接口
    void registerResource(RegisterResp& resp, const RegisterReq& req);

    //处理机器注销接口
    void unRegister(UnRegisterResp& resp, const UnRegisterReq& req);

    //处理机器心跳接口
    void heartbeat(HeartbeatResp& resp, const HeartbeatReq& req);

    //处理上报资源接口
    void reportResource(ReportResourceResp& resp, const ReportResourceReq& req);

    //上报统计信息
    void ReportStatisticsInfo(ReportStatisticsInfoResp& resp, const ReportStatisticsInfoReq& req);

    //获取统计信息
    void GetStatisticsInfo(GetStatisticsInfoResp& resp, const GetStatisticsInfoReq& req);

    //检查cpu资源是否足够
    ErrCode::type checkCpuResource(const InitQubitsReq& req, std::string& addr, std::string& rpcAddr, int& rpcPort, std::vector<std::string>& hosts);

    //检查gpu资源是否足够
    ErrCode::type checkGpuResource(const InitQubitsReq& req, std::string& addr, std::string& rpcAddr, int& rpcPort, std::vector<std::string>& hosts);

    //检查用户自定义资源
    ErrCode::type checkFixedResource(const InitQubitsReq& req, const std::vector<std::string>& hosts, std::string& addr, std::string& rpcAddr, int& rpcPort);

    //定时清理资源
    void timerCleanResource();

    //获取所有的资源
    void getResource(std::map<std::string, DeviceDetail>& devlist);

private:
    bool checkRegisterParam(const RegisterReq& req, RegisterResp& resp);

    //设置rpc返回值
    void setBase(BaseCode& base, const ErrCode::type& code);

    //清理资源
    void cleanResource(const int timeOutDuration = 0);

    //计算qubitNum个的qubit所需的内存，单位：byte
    long long calcQubitBytes(const InitQubitsReq& req, const int numranks);

    //获取启动mpi的进程数
    int getNumRanks(const int numQubits, const int hostSize);
    
private:
    std::atomic<unsigned int> m_indexRand;
    std::mutex m_mutex;
    std::map<std::string, std::shared_ptr<CResource>> m_resourceList;

    std::mutex m_mutexSis;
    std::map<std::string, StatisticsInfo> m_sisList;
};

#endif
