#ifndef __H__BASE__H__
#define __H__BASE__H__

#include <string>
#include <sstream>
#include <thread>
#include <transport/TTransportException.h>
#include "interface/ecode_constants.h"
#include "interface/ecode_types.h"

#define CALL_WITH_SERVICE(CALL, req) \
int retry = 3; \
do { \
    try { \
        CALL; \
        if (resp.base.code == ErrCode::COM_SUCCESS) { \
            break; \
        } \
    } catch (const TTransportException& e) { \
        std::string reqbuf = getPrint(req); \
        LOG(ERROR) << "call exception(reqbuf:" << reqbuf << ",err:" << e.what() << ")."; \
        setBase(resp.base, ErrCode::type::COM_EXCEPTION); \
        if (isNeedReConnectCode(e)) { \
            std::this_thread::sleep_for(std::chrono::seconds(1)); \
            reInit(); \
            continue; \
        } \
        break; \
    } catch (...) { \
        std::string reqbuf = getPrint(req); \
        LOG(ERROR) << "call other exception(reqbuf:" << reqbuf << ")."; \
        setBase(resp.base, ErrCode::type::COM_EXCEPTION); \
        break; \
    } \
} while(retry-- > 0);

class CBase
{
public:
    CBase();
    virtual ~CBase();

    //设置rpc返回值
    void setBase(BaseCode& base, const ErrCode::type& code);

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

    //打印rpc的结构
    template <typename T>
    std::string getPrint(const T& req);
};

template <typename T>
inline std::string CBase::getPrint(const T& req)
{
    std::stringstream os("");
    req.printTo(os);
    return os.str();
}

#endif
