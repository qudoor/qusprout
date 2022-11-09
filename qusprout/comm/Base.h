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

#ifndef __H__BASE__H__
#define __H__BASE__H__

#include <string>
#include <sstream>
#include <thread>
#include <transport/TTransportException.h>
#include "interface/ecode_constants.h"
#include "interface/ecode_types.h"
#include "common/qulog.h"

#define CALL_WITH_SERVICE(CALL, req) \
int retry = 3; \
do { \
    try { \
        CALL; \
        if (resp.base.code != ErrCode::type::COM_MEM_NOT_ENOUGH && resp.base.code != ErrCode::type::COM_NOT_RESOURCE) { \
            break; \
        } \
    } catch (const TTransportException& e) { \
        LOG(ERROR) << "call exception(req:" << getPrint(req) << ",err:" << e.what() << ")."; \
        setBase(resp.base, ErrCode::type::COM_EXCEPTION); \
        if (isNeedReConnectCode(e)) { \
            std::this_thread::sleep_for(std::chrono::seconds(1)); \
            reInit(); \
            continue; \
        } \
        break; \
    } catch (...) { \
        LOG(ERROR) << "call other exception(req:" << getPrint(req) << ")."; \
        setBase(resp.base, ErrCode::type::COM_EXCEPTION); \
        break; \
    } \
} while(retry-- > 0);

#define CALL_WITH_TRY_SERVICE(CALL, req) \
try { \
    CALL; \
} catch (const TTransportException& e) { \
    LOG(ERROR) << "call exception(req:" << getPrint(req) << ",err:" << e.what() << ")."; \
    setBase(resp.base, ErrCode::type::COM_EXCEPTION); \
} catch (...) { \
    LOG(ERROR) << "call other exception(req:" << getPrint(req) << ")."; \
    setBase(resp.base, ErrCode::type::COM_EXCEPTION); \
}

class CBase
{
public:
    CBase();
    virtual ~CBase();

    //获取本机随机可用端口
    int getLocalPort();

    //开机清理task
    int killTask(const std::string processName);

    //等待指定端口监听完成
    bool waitListenFinish(const int port, const int milliseconds = 30000);

    //tpc是否需要重连
    bool isNeedReConnectCode(const apache::thrift::transport::TTransportException& e);

    //判断传入字符串是否全数字
    bool isDigitStr(char* str);
};

//设置rpc返回值
void setBase(BaseCode& base, const ErrCode::type& code, const std::string& msg = "");

//打印rpc的结构
template <typename T>
inline std::string getPrint(const T& req)
{
    std::stringstream os("");
    req.printTo(os);
    return os.str();
}

class ResourceData 
{
public:
    void reset();
    ResourceData operator+=(const ResourceData& temp);

public:
    long long int cpubytes{0};
};

//获取当前毫秒时间戳
long long int getCurrMs();

#endif
