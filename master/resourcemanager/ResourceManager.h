#ifndef __H__RESOURCEMANAGER__H__
#define __H__RESOURCEMANAGER__H__

#include <map>
#include <mutex>
#include <memory>
#include <string>
#include <atomic>
#include "comm/SelfStruct.h"
#include "comm/Base.h"
#include "interface/ecode_types.h"
#include "interface/resource_types.h"
#include "interface/qusproutdata_types.h"
#include "interface/qurootresource_types.h"

//机器资源
class CResource
{
public:
    CResource(const RegisterReq& req);

    //更新注册信息
    void updateResource(const RegisterReq& req);

    //更新心跳资源等信息
    void updateResource(const HeartbeatReq& req);

    //获取地址
    void getAddr(RpcConnectInfo& addr);

    //获取地址
    std::string getAddr();

    //是否超时
    bool isTimeout();

    //清理该资源对应的任务
    void cleanResourceOfTask();

    //更新时间
    void updateTime();

    //判断是否是该资源
    bool isEqualResource(const std::string& addr);
    
public:
    //创建时间
    long long int m_createtime{0};

    //更新时间
    long long int m_updatetime{0};

    //资源信息
    RegisterReq m_resource;
};

class CResourceManager
{
public:
    CResourceManager();
    ~CResourceManager();

    //处理机器注册接口
    void registerResource(RegisterResp& resp, const RegisterReq& req);

    //处理机器注销接口
    void unRegister(UnRegisterResp& resp, const UnRegisterReq& req);

    //处理机器心跳接口
    void heartbeat(HeartbeatResp& resp, const HeartbeatReq& req);

    //检查资源是否足够
    ErrCode::type getResource(const InitQubitsReq& req, std::string& resourceid, RpcConnectInfo& addr, std::vector<std::string>& hosts, ResourceData& resourcebytes);

    //定时清理资源
    void cleanResource();

    //清空资源
    void cleanAllResource();

    //获取所有的资源
    void getAllResource(std::map<std::string, DeviceDetail>& devlist);

private:
    //获取单个gpu资源的信息
    ErrCode::type getSignalGpuResource(const InitQubitsReq& req, const std::map<std::string, ResourceData>& useresource, std::string& resourceid, RpcConnectInfo& addr, std::vector<std::string>& hosts, ResourceData& resourcebytes);

    //获取mpi多个cpu资源的信息
    ErrCode::type getMpiCpuResource(const InitQubitsReq& req, const std::map<std::string, ResourceData>& useresource, std::string& resourceid, RpcConnectInfo& addr, std::vector<std::string>& hosts, ResourceData& resourcebytes);

    //获取单个cpu资源的信息
    ErrCode::type getSignalCpuResource(const InitQubitsReq& req, const std::map<std::string, ResourceData>& useresource, std::string& resourceid, RpcConnectInfo& addr, std::vector<std::string>& hosts, ResourceData& resourcebytes);

    //计算单机需要的内存
    long long calcSingleBytes(const int qubits, const int density);

    //计算mpi需要的内存
    long long calcMpiBytes(const int qubits, const int density, const int numranks);

    //获取启动mpi的进程数
    int getNumRanks(const int qubits);
    
private:
    std::atomic_long m_index{0};
    std::mutex m_mutex;
    std::map<std::string, std::shared_ptr<CResource>> m_resourceList;
};

#endif
