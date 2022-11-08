#include <transport/TSocket.h>
#include <protocol/TBinaryProtocol.h>
#include <protocol/TMultiplexedProtocol.h>
#include "QuRootClient.h"
#include "common/qulog.h"
#include "config/Config.h"
#include "common/Singleton.h"

CQurootClient::CQurootClient()
{
}

CQurootClient::~CQurootClient()
{
    close();
}

void CQurootClient::close()
{
    if (m_transport != nullptr)
    {
        m_transport->close();
        m_transport = nullptr;
    }
}

//重新初始化客户端
int CQurootClient::connect()
{
    try
    {
        std::shared_ptr<TSocket> socket = std::make_shared<TSocket>(SINGLETON(CConfig)->m_qurootAddr, SINGLETON(CConfig)->m_qurootPort); 
        socket->setConnTimeout(SINGLETON(CConfig)->m_clientConnTimeout);
        socket->setRecvTimeout(SINGLETON(CConfig)->m_clientSendTimeout);
        socket->setSendTimeout(SINGLETON(CConfig)->m_clientRecvTimeout);
        m_transport = std::make_shared<TBufferedTransport>(socket);
        std::shared_ptr<TProtocol> protocol = std::make_shared<TBinaryProtocol>(m_transport);
        std::shared_ptr<TMultiplexedProtocol> quest = std::make_shared<TMultiplexedProtocol>(protocol, "QuRootServer");
        m_transport->open();

        m_client = std::make_shared<QuRootServerClient>(quest);
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
void CQurootClient::resourceRegister(ResourceRegisterResp& resp, const ResourceRegisterReq& req)
{
    LOG(INFO) << "request resourceRegister(req:" << getPrint(req) << ").";

    CALL_WITH_TRY_SERVICE(m_client->resourceRegister(resp, req), req);
    LOG(INFO) << "response resourceRegister(resp:" << getPrint(resp) << ").";
}

//处理机器注销接口
void CQurootClient::unResourceRegister(UnResourceRegisterResp& resp, const UnResourceRegisterReq& req)
{
    LOG(INFO) << "request unResourceRegister(req:" << getPrint(req) << ").";

    CALL_WITH_TRY_SERVICE(m_client->unResourceRegister(resp, req), req);
    LOG(INFO) << "response unResourceRegister(resp:" << getPrint(resp) << ").";
}

//处理机器心跳接口
void CQurootClient::heart(HeartResp& resp, const HeartReq& req)
{
    CALL_WITH_TRY_SERVICE(m_client->heart(resp, req), req);
}
