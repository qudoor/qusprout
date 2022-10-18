#include <transport/TSocket.h>
#include <protocol/TBinaryProtocol.h>
#include <protocol/TMultiplexedProtocol.h>
#include "MasterClient.h"
#include "common/qulog.h"
#include "config/Config.h"
#include "common/Singleton.h"

CMasterClient::CMasterClient()
{

}

CMasterClient::~CMasterClient()
{
    close();
}

void CMasterClient::close()
{
    if (m_transport != nullptr)
    {
        m_transport->close();
        m_transport = nullptr;
    }
}

//连接客户端
int CMasterClient::connect()
{
    try
    {
        std::shared_ptr<TSocket> socket = std::make_shared<TSocket>(SINGLETON(CConfig)->m_masterAddr, SINGLETON(CConfig)->m_masterPort); 
        socket->setConnTimeout(SINGLETON(CConfig)->m_clientConnTimeout);
        socket->setRecvTimeout(SINGLETON(CConfig)->m_clientSendTimeout);
        socket->setSendTimeout(SINGLETON(CConfig)->m_clientRecvTimeout);
        m_transport = std::make_shared<TBufferedTransport>(socket);
        std::shared_ptr<TProtocol> protocol = std::make_shared<TBinaryProtocol>(m_transport);
        std::shared_ptr<TMultiplexedProtocol> quest = std::make_shared<TMultiplexedProtocol>(protocol, "MasterServer");
        m_transport->open();

        m_client = std::make_shared<MasterServerClient>(quest);
    }
    catch(const TTransportException& e)
    {
        LOG(ERROR) << "init exception(err:" << e.what() << ").";
        return -1;
    }
    catch(...)
    {
        LOG(ERROR) << "init other exception.";
        return -2;
    }
    
    return 0;
}

//处理机器注册接口
void CMasterClient::registerResource(RegisterResp& resp, const RegisterReq& req)
{
    LOG(INFO) << "request registerResource(req:" << getPrint(req) << ").";

    CALL_WITH_TRY_SERVICE(m_client->registerResource(resp, req), req);
    LOG(INFO) << "response registerResource(resp:" << getPrint(resp) << ").";
}

//处理机器注销接口
void CMasterClient::unRegister(UnRegisterResp& resp, const UnRegisterReq& req)
{
    LOG(INFO) << "request unRegister(req:" << getPrint(req) << ").";

    CALL_WITH_TRY_SERVICE(m_client->unRegister(resp, req), req);
    LOG(INFO) << "response unRegister(resp:" << getPrint(resp) << ").";
}

//处理机器心跳接口
void CMasterClient::heartbeat(HeartbeatResp& resp, const HeartbeatReq& req)
{
    CALL_WITH_TRY_SERVICE(m_client->heartbeat(resp, req), req);
}
