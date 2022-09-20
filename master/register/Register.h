#ifndef __H__REGISTER__H__
#define __H__REGISTER__H__

#include <string>
#include <memory>
#include <time.h>
#include <atomic>
#include <mutex>
#include "rpcclient/QuRootClient.h"
#include "common/SystemHandle.h"

class CRegister : public CBase
{
public:
    CRegister();
    virtual ~CRegister();

    //向quroot注册
    int resourceRegister();

    //处理机器注销接口
    int unResourceRegister();

    //处理机器心跳接口
    int heart();

private:
    std::mutex m_mutex;
    std::string m_resourceid{""};
};

#endif
