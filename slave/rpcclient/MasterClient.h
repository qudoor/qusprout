#ifndef __H__MASTERCLIENT__H__
#define __H__MASTERCLIENT__H__

#include <string>
#include <memory>
#include <transport/TBufferTransports.h>
#include <transport/TTransportException.h>
#include <transport/TServerSocket.h>
#include "interface/MasterServer.h"
#include "interface/resource_types.h"
#include "comm/Base.h"

using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;

class CMasterClient : public CBase
{
public:
    CMasterClient();
    virtual ~CMasterClient();

    //连接客户端
    int connect();

    //处理机器注册接口
    void registerResource(RegisterResp& resp, const RegisterReq& req);

    //处理机器注销接口
    void unRegister(UnRegisterResp& resp, const UnRegisterReq& req);

    //处理机器心跳接口
    void heartbeat(HeartbeatResp& resp, const HeartbeatReq& req);

private:
    //关闭客户端
    void close();

private:
    std::shared_ptr<MasterServerClient> m_client{nullptr};
    std::shared_ptr<TTransport> m_transport{nullptr};
};

#endif 