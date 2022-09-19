#ifndef __H__BASE__H__
#define __H__BASE__H__

#include <string>
#include <sstream>
#include <transport/TTransportException.h>
#include "interface/ecode_constants.h"
#include "interface/ecode_types.h"

class CBase
{
public:
    CBase();
    virtual ~CBase();

    //设置rpc返回值
    void setBase(BaseCode& base, const ErrCode::type& code);

    //tpc是否需要重连
    bool isNeedReConnectCode(const apache::thrift::transport::TTransportException& e);

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
