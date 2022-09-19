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

int CMasterClient::init(const std::string& addr, const int port)
{
    m_addr = addr;
    m_port = port;
    return reInit(false);
}

void CMasterClient::close()
{
    if (m_transport != nullptr)
    {
        m_transport->close();
        m_transport = nullptr;
    }
}

//重新初始化客户端
int CMasterClient::reInit(const bool isprint)
{
    try
    {
        close();
        std::shared_ptr<TSocket> socket = std::make_shared<TSocket>(m_addr, m_port); 
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
        if (isprint)
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
    std::string reqbuf = getPrint<RegisterReq>(req);
    LOG(INFO) << "request registerResource(req:" << reqbuf << ").";

    try
    {
        m_client->registerResource(resp, req);
    }
    catch(const TTransportException& e)
    {
        bool rnt = isNeedReConnectCode(e);
        LOG(ERROR) << "registerResource exception(seq:" << req.seq << ",rnt:" << rnt << ",err:" << e.what() << ").";
        if (rnt)
        {   
            reInit();
        }
        setBase(resp.base, ErrCode::type::COM_EXCEPTION);
    }
    catch(...)
    {
        LOG(ERROR) << "registerResource other exception(seq:" << req.seq << ").";
        setBase(resp.base, ErrCode::type::COM_EXCEPTION);
    }
    
    LOG(INFO) << "response registerResource(seq:" << req.seq << ",resp:" << getPrint<RegisterResp>(resp) << ").";
}

//处理机器注销接口
void CMasterClient::unRegister(UnRegisterResp& resp, const UnRegisterReq& req)
{
    std::string reqbuf = getPrint<UnRegisterReq>(req);
    LOG(INFO) << "request unRegister(req:" << reqbuf << ").";

    try
    {
        m_client->unRegister(resp, req);
    }
    catch(const TTransportException& e)
    {
        bool rnt = isNeedReConnectCode(e);
        LOG(ERROR) << "unRegister exception(seq:" << req.seq << ",rnt:" << rnt << ",err:" << e.what() << ").";
        if (rnt)
        {   
            reInit();
        }
        setBase(resp.base, ErrCode::type::COM_EXCEPTION);
    }
    catch(...)
    {
        LOG(ERROR) << "unRegister other exception(seq:" << req.seq << ").";
        setBase(resp.base, ErrCode::type::COM_EXCEPTION);
    }
    
    LOG(INFO) << "response unRegister(seq:" << req.seq << ",resp:" << getPrint<UnRegisterResp>(resp) << ").";
}

//处理机器心跳接口
void CMasterClient::heartbeat(HeartbeatResp& resp, const HeartbeatReq& req)
{
    try
    {
        m_client->heartbeat(resp, req);
    }
    catch(const TTransportException& e)
    {
        bool rnt = isNeedReConnectCode(e);
        std::string reqbuf = getPrint<HeartbeatReq>(req);
        LOG(ERROR) << "heartbeat exception(req:" << reqbuf << ",rnt:" << rnt << ",err:" << e.what() << ").";
        if (rnt)
        {   
            reInit();
        }
        setBase(resp.base, ErrCode::type::COM_EXCEPTION);
    }
    catch(...)
    {
        std::string reqbuf = getPrint<HeartbeatReq>(req);
        LOG(ERROR) << "heartbeat other exception(req:" << reqbuf << ").";
        setBase(resp.base, ErrCode::type::COM_EXCEPTION);
    }
}

//处理上报资源接口
void CMasterClient::reportResource(ReportResourceResp& resp, const ReportResourceReq& req)
{
    try
    {
        m_client->reportResource(resp, req);
    }
    catch(const TTransportException& e)
    {
        bool rnt = isNeedReConnectCode(e);
        std::string reqbuf = getPrint<ReportResourceReq>(req);
        LOG(ERROR) << "reportResource exception(req:" << reqbuf << ",rnt:" << rnt << ",err:" << e.what() << ").";
        if (rnt)
        {   
            reInit();
        }
        setBase(resp.base, ErrCode::type::COM_EXCEPTION);
    }
    catch(...)
    {
        std::string reqbuf = getPrint<ReportResourceReq>(req);
        LOG(ERROR) << "reportResource other exception(req:" << reqbuf << ").";
        setBase(resp.base, ErrCode::type::COM_EXCEPTION);
    }
}

//上报统计信息
void CMasterClient::ReportStatisticsInfo(ReportStatisticsInfoResp& resp, const ReportStatisticsInfoReq& req)
{
    try
    {
        m_client->ReportStatisticsInfo(resp, req);
    }
    catch(const TTransportException& e)
    {
        bool rnt = isNeedReConnectCode(e);
        std::string reqbuf = getPrint<ReportStatisticsInfoReq>(req);
        LOG(ERROR) << "ReportStatisticsInfo exception(req:" << reqbuf << ",rnt:" << rnt << ",err:" << e.what() << ").";
        if (rnt)
        {   
            reInit();
        }
        setBase(resp.base, ErrCode::type::COM_EXCEPTION);
    }
    catch(...)
    {
        std::string reqbuf = getPrint<ReportStatisticsInfoReq>(req);
        LOG(ERROR) << "ReportStatisticsInfo other exception(req:" << reqbuf << ").";
        setBase(resp.base, ErrCode::type::COM_EXCEPTION);
    }
}

//获取统计信息
void CMasterClient::GetStatisticsInfo(GetStatisticsInfoResp& resp, const GetStatisticsInfoReq& req)
{
    try
    {
        m_client->GetStatisticsInfo(resp, req);
    }
    catch(const TTransportException& e)
    {
        bool rnt = isNeedReConnectCode(e);
        std::string reqbuf = getPrint<GetStatisticsInfoReq>(req);
        LOG(ERROR) << "GetStatisticsInfo exception(req:" << reqbuf << ",rnt:" << rnt << ",err:" << e.what() << ").";
        if (rnt)
        {   
            reInit();
        }
        setBase(resp.base, ErrCode::type::COM_EXCEPTION);
    }
    catch(...)
    {
        std::string reqbuf = getPrint<GetStatisticsInfoReq>(req);
        LOG(ERROR) << "GetStatisticsInfo other exception(req:" << reqbuf << ").";
        setBase(resp.base, ErrCode::type::COM_EXCEPTION);
    }
}
