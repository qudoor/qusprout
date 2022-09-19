#include "Base.h"

CBase::CBase()
{

}

CBase::~CBase()
{

}

//设置rpc返回值
void CBase::setBase(BaseCode& base, const ErrCode::type& code)
{
    base.__set_code(code);
    auto ptr = g_ecode_constants.ErrMsg.find(code);
    base.__set_msg(ptr->second);
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