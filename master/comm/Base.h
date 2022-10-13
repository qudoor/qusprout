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

    //tpc是否需要重连
    bool isNeedReConnectCode(const apache::thrift::transport::TTransportException& e);
};

//设置rpc返回值
void setBase(BaseCode& base, const ErrCode::type& code);

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
    long long int gpubytes{0};
};

//获取当前毫秒时间戳
long long int getCurrMs();

#endif
