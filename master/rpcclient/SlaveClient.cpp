#include <transport/TSocket.h>
#include <protocol/TBinaryProtocol.h>
#include <protocol/TMultiplexedProtocol.h>
#include "SlaveClient.h"
#include "common/qulog.h"
#include "config/Config.h"
#include "common/Singleton.h"

CSlaveClient::CSlaveClient()
{

}

CSlaveClient::~CSlaveClient()
{
    close();
}

bool CSlaveClient::isInit()
{
    return m_isInit;
}

int CSlaveClient::init(const std::string& addr, const int port)
{
    m_addr = addr;
    m_port = port;
    return reInit();
}

void CSlaveClient::close()
{
    if (m_transport != nullptr)
    {
        m_transport->close();
        m_transport = nullptr;
    }
}

//重新初始化客户端
int CSlaveClient::reInit()
{
    try
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        close();
        std::shared_ptr<TSocket> socket = std::make_shared<TSocket>(m_addr, m_port); 
        socket->setConnTimeout(SINGLETON(CConfig)->m_clientConnTimeout);
        socket->setRecvTimeout(SINGLETON(CConfig)->m_clientSendTimeout);
        socket->setSendTimeout(SINGLETON(CConfig)->m_clientRecvTimeout);
        m_transport = std::make_shared<TBufferedTransport>(socket);
        std::shared_ptr<TProtocol> protocol = std::make_shared<TBinaryProtocol>(m_transport);
        std::shared_ptr<TMultiplexedProtocol> quest = std::make_shared<TMultiplexedProtocol>(protocol, "SlaveServer");
        m_transport->open();

        m_client = std::make_shared<SlaveServerClient>(quest);
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

//qubit初始化
void CSlaveClient::initQubits(InitQubitsResp& resp, const InitQubitsReq& req)
{
    std::string reqbuf = getPrint<InitQubitsReq>(req);
    LOG(INFO) << "request initQubits(req:" << reqbuf << ").";

    try
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        m_client->initQubits(resp, req); 
        if (resp.base.code == ErrCode::type::COM_SUCCESS)
        {
            m_isInit = true;
        }
    }
    catch(const TTransportException& e)
    {
        bool rnt = isNeedReConnectCode(e);
        LOG(ERROR) << "initQubits exception(id:" << req.id << ",rnt:" << rnt << ",err:" << e.what() << ").";
        if (rnt)
        {   
            reInit();
        }
        setBase(resp.base, ErrCode::type::COM_EXCEPTION);
    }
    catch(...)
    {
        LOG(ERROR) << "initQubits other exception(id:" << req.id << ").";
        setBase(resp.base, ErrCode::type::COM_EXCEPTION);
    }
    
    LOG(INFO) << "response initQubits(id:" << req.id << ",resp:" << getPrint<InitQubitsResp>(resp) << ").";
}

//发送任务
void CSlaveClient::sendCircuitCmd(SendCircuitCmdResp& resp, const SendCircuitCmdReq& req)
{
    try
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        m_client->sendCircuitCmd(resp, req);
    }
    catch(const TTransportException& e)
    {
        bool rnt = isNeedReConnectCode(e);
        std::string reqbuf = getPrint<SendCircuitCmdReq>(req);
        LOG(ERROR) << "sendCircuitCmd exception(req:" << reqbuf << ",rnt:" << rnt << ",err:" << e.what() << ").";
        if (rnt)
        {   
            reInit();
        }
        setBase(resp.base, ErrCode::type::COM_EXCEPTION);
    }
    catch(...)
    {
        std::string reqbuf = getPrint<SendCircuitCmdReq>(req);
        LOG(ERROR) << "sendCircuitCmd other exception(req:" << reqbuf << ").";
        setBase(resp.base, ErrCode::type::COM_EXCEPTION);
    }
}

//取消任务
void CSlaveClient::cancelCmd(CancelCmdResp& resp, const CancelCmdReq& req)
{
    std::string reqbuf = getPrint<CancelCmdReq>(req);
    LOG(INFO) << "request cancelCmd(req:" << reqbuf << ").";

    try
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        m_client->cancelCmd(resp, req);
    }
    catch(const TTransportException& e)
    {
        bool rnt = isNeedReConnectCode(e);
        LOG(ERROR) << "cancelCmd exception(id:" << req.id << ",rnt:" << rnt << ",err:" << e.what() << ").";
        if (rnt)
        {   
            reInit();
        }
        setBase(resp.base, ErrCode::type::COM_EXCEPTION);
    }
    catch(...)
    {
        LOG(ERROR) << "cancelCmd other exception(id:" << req.id << ").";
        setBase(resp.base, ErrCode::type::COM_EXCEPTION);
    }
    
    LOG(INFO) << "response cancelCmd(id:" << req.id << ",resp:" << getPrint<CancelCmdResp>(resp) << ").";
}

//获取振幅
void CSlaveClient::getProbAmp(GetProbAmpResp& resp, const GetProbAmpReq& req)
{
    try
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        m_client->getProbAmp(resp, req);
    }
    catch(const TTransportException& e)
    {
        bool rnt = isNeedReConnectCode(e);
        std::string reqbuf = getPrint<GetProbAmpReq>(req);
        LOG(ERROR) << "getProbAmp exception(req:" << reqbuf << ",rnt:" << rnt << ",err:" << e.what() << ").";
        if (rnt)
        {   
            reInit();
        }
        setBase(resp.base, ErrCode::type::COM_EXCEPTION);
    }
    catch(...)
    {
        std::string reqbuf = getPrint<GetProbAmpReq>(req);
        LOG(ERROR) << "getProbAmp other exception(req:" << reqbuf << ").";
        setBase(resp.base, ErrCode::type::COM_EXCEPTION);
    }
}

//获取当前qubit的概率
void CSlaveClient::getProbOfOutcome(GetProbOfOutcomeResp& resp, const GetProbOfOutcomeReq& req)
{
    try
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        m_client->getProbOfOutcome(resp, req);
    }
    catch(const TTransportException& e)
    {
        bool rnt = isNeedReConnectCode(e);
        std::string reqbuf = getPrint<GetProbOfOutcomeReq>(req);
        LOG(ERROR) << "getProbOfOutcome exception(req:" << reqbuf << ",rnt:" << rnt << ",err:" << e.what() << ").";
        if (rnt)
        {   
            reInit();
        }
        setBase(resp.base, ErrCode::type::COM_EXCEPTION);
    }
    catch(...)
    {
        std::string reqbuf = getPrint<GetProbOfOutcomeReq>(req);
        LOG(ERROR) << "getProbOfOutcome other exception(req:" << reqbuf << ").";
        setBase(resp.base, ErrCode::type::COM_EXCEPTION);
    }
}

//获取所有qubit的概率
void CSlaveClient::getProbOfAllOutcome(GetProbOfAllOutcomResp& resp, const GetProbOfAllOutcomReq& req)
{
    try
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        m_client->getProbOfAllOutcome(resp, req);
    }
    catch(const TTransportException& e)
    {
        bool rnt = isNeedReConnectCode(e);
        std::string reqbuf = getPrint<GetProbOfAllOutcomReq>(req);
        LOG(ERROR) << "getProbOfAllOutcome exception(req:" << reqbuf << ",rnt:" << rnt << ",err:" << e.what() << ").";
        if (rnt)
        {   
            reInit();
        }
        setBase(resp.base, ErrCode::type::COM_EXCEPTION);
    }
    catch(...)
    {
        std::string reqbuf = getPrint<GetProbOfAllOutcomReq>(req);
        LOG(ERROR) << "getProbOfAllOutcome other exception(req:" << reqbuf << ").";
        setBase(resp.base, ErrCode::type::COM_EXCEPTION);
    }
}

//获取所有的计算结果
void CSlaveClient::getAllState(GetAllStateResp& resp, const GetAllStateReq& req)
{
    try
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        m_client->getAllState(resp, req);
    }
    catch(const TTransportException& e)
    {
        bool rnt = isNeedReConnectCode(e);
        std::string reqbuf = getPrint<GetAllStateReq>(req);
        LOG(ERROR) << "getAllState exception(req:" << reqbuf << ",rnt:" << rnt << ",err:" << e.what() << ").";
        if (rnt)
        {   
            reInit();
        }
        setBase(resp.base, ErrCode::type::COM_EXCEPTION);
    }
    catch(...)
    {
        std::string reqbuf = getPrint<GetAllStateReq>(req);
        LOG(ERROR) << "getAllState other exception(req:" << reqbuf << ").";
        setBase(resp.base, ErrCode::type::COM_EXCEPTION);
    }
}

//执行任务
void CSlaveClient::run(RunCircuitResp& resp, const RunCircuitReq& req)
{
    try
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        m_client->run(resp, req);
    }
    catch(const TTransportException& e)
    {
        bool rnt = isNeedReConnectCode(e);
        std::string reqbuf = getPrint<RunCircuitReq>(req);
        LOG(ERROR) << "run exception(req:" << reqbuf << ",rnt:" << rnt << ",err:" << e.what() << ").";
        if (rnt)
        {   
            reInit();
        }
        setBase(resp.base, ErrCode::type::COM_EXCEPTION);
    }
    catch(...)
    {
        std::string reqbuf = getPrint<RunCircuitReq>(req);
        LOG(ERROR) << "run other exception(req:" << reqbuf << ").";
        setBase(resp.base, ErrCode::type::COM_EXCEPTION);
    }
}

//对部分量子比特应用量子傅立叶变换
void CSlaveClient::applyQFT(ApplyQFTResp& resp, const ApplyQFTReq& req)
{
    try
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        m_client->applyQFT(resp, req);
    }
    catch(const TTransportException& e)
    {
        bool rnt = isNeedReConnectCode(e);
        std::string reqbuf = getPrint<ApplyQFTReq>(req);
        LOG(ERROR) << "applyQFT exception(req:" << reqbuf << ",rnt:" << rnt << ",err:" << e.what() << ").";
        if (rnt)
        {   
            reInit();
        }
        setBase(resp.base, ErrCode::type::COM_EXCEPTION);
    }
    catch(...)
    {
        std::string reqbuf = getPrint<ApplyQFTReq>(req);
        LOG(ERROR) << "applyQFT other exception(req:" << reqbuf << ").";
        setBase(resp.base, ErrCode::type::COM_EXCEPTION);
    }
}

//对所有量子比特应用量子傅立叶变换
void CSlaveClient::applyFullQFT(ApplyFullQFTResp& resp, const ApplyFullQFTReq& req)
{
    try
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        m_client->applyFullQFT(resp, req);
    }
    catch(const TTransportException& e)
    {
        bool rnt = isNeedReConnectCode(e);
        std::string reqbuf = getPrint<ApplyFullQFTReq>(req);
        LOG(ERROR) << "applyFullQFT exception(req:" << reqbuf << ",rnt:" << rnt << ",err:" << e.what() << ").";
        if (rnt)
        {   
            reInit();
        }
        setBase(resp.base, ErrCode::type::COM_EXCEPTION);
    }
    catch(...)
    {
        std::string reqbuf = getPrint<ApplyFullQFTReq>(req);
        LOG(ERROR) << "applyFullQFT other exception(req:" << reqbuf << ").";
        setBase(resp.base, ErrCode::type::COM_EXCEPTION);
    }
}

//获取泡利算子乘积的期望值
void CSlaveClient::getExpecPauliProd(GetExpecPauliProdResp& resp, const GetExpecPauliProdReq& req)
{
    try
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        m_client->getExpecPauliProd(resp, req);
    }
    catch(const TTransportException& e)
    {
        bool rnt = isNeedReConnectCode(e);
        std::string reqbuf = getPrint<GetExpecPauliProdReq>(req);
        LOG(ERROR) << "getExpecPauliProd exception(req:" << reqbuf << ",rnt:" << rnt << ",err:" << e.what() << ").";
        if (rnt)
        {   
            reInit();
        }
        setBase(resp.base, ErrCode::type::COM_EXCEPTION);
    }
    catch(...)
    {
        std::string reqbuf = getPrint<GetExpecPauliProdReq>(req);
        LOG(ERROR) << "getExpecPauliProd other exception(req:" << reqbuf << ").";
        setBase(resp.base, ErrCode::type::COM_EXCEPTION);
    }
}

//获取泡利算子乘积之和的期望值
void CSlaveClient::getExpecPauliSum(GetExpecPauliSumResp& resp, const GetExpecPauliSumReq& req)
{
    try
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        m_client->getExpecPauliSum(resp, req);
    }
    catch(const TTransportException& e)
    {
        bool rnt = isNeedReConnectCode(e);
        std::string reqbuf = getPrint<GetExpecPauliSumReq>(req);
        LOG(ERROR) << "getExpecPauliSum exception(req:" << reqbuf << ",rnt:" << rnt << ",err:" << e.what() << ").";
        if (rnt)
        {   
            reInit();
        }
        setBase(resp.base, ErrCode::type::COM_EXCEPTION);
    }
    catch(...)
    {
        std::string reqbuf = getPrint<GetExpecPauliSumReq>(req);
        LOG(ERROR) << "getExpecPauliSum other exception(req:" << reqbuf << ").";
        setBase(resp.base, ErrCode::type::COM_EXCEPTION);
    }
}

