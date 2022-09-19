#ifndef __H__REGISTER__H__
#define __H__REGISTER__H__

#include <string>
#include <memory>
#include <time.h>
#include <atomic>
#include <mutex>
#include "rpcclient/MasterClient.h"
#include "common/SystemHandle.h"

class CRegister : public CBase
{
public:
    CRegister();
    virtual ~CRegister();

    //初始化
    int init(const int timeout = 10000);
    
    //停止注册
    void stop();

    //向master注册
    int registerToMaster();

    //处理机器注销接口
    int unRegisterToMaster();

    //处理机器心跳接口
    int heartbeatToMaster();

    //处理上报资源接口
    int reportResourceToMaster();

    //上报统计信息
    void ReportStatisticsInfo();

    //获取hostname
    std::string getHostName();

private:
    //获取流水
    std::string getSeq(const int type);

private:
    bool m_isreg{false};
    std::mutex m_mutex;
    MachineSysInfo m_machine;
    std::shared_ptr<CMasterClient> m_client{nullptr};
    time_t m_lastPrintTime{0};
    std::atomic<unsigned int> m_seqIndex{0};
};

#endif
