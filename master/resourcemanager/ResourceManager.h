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
#include "../common/SystemHandle.h"

//资源信息
typedef struct DeviceResDetail {
    MachineSysInfo machine;
    MemUseInfo resource;
} DevResDetail;

//机器资源
class CResource
{
public:
    CResource();

    //更新时间
    void updateTime();
    
public:
    //创建时间
    long long int m_createtime{0};

    //更新时间
    long long int m_updatetime{0};

    //资源信息
    DevResDetail m_resource;
};

class CResourceManager
{
public:
    CResourceManager();
    ~CResourceManager();

    //初始化本地资源
    bool init();

    //检查资源是否足够
    ErrCode::type getResource(const InitQubitsReq& req, std::string& resourceid, ResourceData& resourcebytes);

    //获取所有的资源
    void getAllResource(std::map<std::string, DeviceDetail>& devlist);

private:
    //获取mpi多个cpu资源的信息
    ErrCode::type getMpiCpuResource(const InitQubitsReq& req, const std::map<std::string, ResourceData>& useresource, std::string& resourceid, ResourceData& resourcebytes);

    //获取单个cpu资源的信息
    ErrCode::type getSignalCpuResource(const InitQubitsReq& req, const std::map<std::string, ResourceData>& useresource, std::string& resourceid, ResourceData& resourcebytes);

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
