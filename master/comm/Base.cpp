#include <sys/time.h>
#include "Base.h"

CBase::CBase()
{

}

CBase::~CBase()
{

}

//tpc是否需要重连
bool CBase::isNeedReConnectCode(const apache::thrift::transport::TTransportException& e)
{
    if (e.getType() == apache::thrift::transport::TTransportException::TTransportExceptionType::CLIENT_DISCONNECT ||
        e.getType() == apache::thrift::transport::TTransportException::TTransportExceptionType::NOT_OPEN ||
        e.getType() == apache::thrift::transport::TTransportException::TTransportExceptionType::END_OF_FILE
    )
    {   
        return true;
    }

    return false;
}

void setBase(BaseCode& base, const ErrCode::type& code)
{
    base.__set_code(code);
    auto ptr = g_ecode_constants.ErrMsg.find(code);
    base.__set_msg(ptr->second);
}

void ResourceData::reset()
{
    cpubytes = 0;
    gpubytes = 0;
}

ResourceData ResourceData::operator+=(const ResourceData& temp)
{
    cpubytes += temp.cpubytes;
    gpubytes += temp.gpubytes;
    return *this;
}

long long int getCurrMs()
{
    struct timeval time;
    gettimeofday(&time, NULL);
    return (time.tv_sec*1000 + time.tv_usec/1000);
}
