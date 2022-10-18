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

    //向master注册
    int registerToMaster();

    //处理机器注销接口
    int unRegisterToMaster();

    //处理机器心跳接口
    int heartbeatToMaster();

private:
    std::mutex m_mutex;
    std::string m_resourceid{""};
};

#endif
