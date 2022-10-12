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
    int ret = 0, retry = 3;
    do
    {
        ret = reInit();
        if (0 == ret)
        {   
            break;
        }
        --retry;
    } while(retry > 0);

    return ret;
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
        close();
        std::shared_ptr<TSocket> socket = std::make_shared<TSocket>(m_addr, m_port); 
        socket->setConnTimeout(SINGLETON(CConfig)->m_clientConnTimeout);
        socket->setRecvTimeout(SINGLETON(CConfig)->m_clientSendTimeout);
        socket->setSendTimeout(SINGLETON(CConfig)->m_clientRecvTimeout);
        m_transport = std::make_shared<TBufferedTransport>(socket);
        std::shared_ptr<TProtocol> protocol = std::make_shared<TBinaryProtocol>(m_transport);
        std::shared_ptr<TMultiplexedProtocol> quest = std::make_shared<TMultiplexedProtocol>(protocol, "QuSproutServer");
        m_transport->open();

        m_client = std::make_shared<QuSproutServerClient>(quest);
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
    LOG(INFO) << "request initQubits(req:" << getPrint(req) << ").";

    CALL_WITH_SERVICE(m_client->initQubits(resp, req), req);
    if (resp.base.code == ErrCode::COM_SUCCESS)
    {
        m_isInit = true;
    }
    
    LOG(INFO) << "response initQubits(id:" << req.id << ",resp:" << getPrint(resp) << ").";
}

//发送任务
void CSlaveClient::sendCircuitCmd(SendCircuitCmdResp& resp, const SendCircuitCmdReq& req)
{
    if (!isInit())
    {
        setBase(resp.base, ErrCode::type::QUROOT_NOT_INIT);
        return;
    }

    CALL_WITH_SERVICE(m_client->sendCircuitCmd(resp, req), req);
}

//取消任务
void CSlaveClient::cancelCmd(CancelCmdResp& resp, const CancelCmdReq& req)
{
    LOG(INFO) << "request cancelCmd(req:" << getPrint(req) << ").";

    if (!isInit())
    {
        setBase(resp.base, ErrCode::type::QUROOT_NOT_INIT);
        return;
    }

    CALL_WITH_SERVICE(m_client->cancelCmd(resp, req), req);
    LOG(INFO) << "response cancelCmd(id:" << req.id << ",resp:" << getPrint(resp) << ").";
}

//获取振幅
void CSlaveClient::getProbAmp(GetProbAmpResp& resp, const GetProbAmpReq& req)
{
    if (!isInit())
    {
        setBase(resp.base, ErrCode::type::QUROOT_NOT_INIT);
        return;
    }

    CALL_WITH_SERVICE(m_client->getProbAmp(resp, req), req);
}

//获取当前qubit的概率
void CSlaveClient::getProbOfOutcome(GetProbOfOutcomeResp& resp, const GetProbOfOutcomeReq& req)
{
    if (!isInit())
    {
        setBase(resp.base, ErrCode::type::QUROOT_NOT_INIT);
        return;
    }

    CALL_WITH_SERVICE(m_client->getProbOfOutcome(resp, req), req);
}

//获取所有qubit的概率
void CSlaveClient::getProbOfAllOutcome(GetProbOfAllOutcomResp& resp, const GetProbOfAllOutcomReq& req)
{
    if (!isInit())
    {
        setBase(resp.base, ErrCode::type::QUROOT_NOT_INIT);
        return;
    }

    CALL_WITH_SERVICE(m_client->getProbOfAllOutcome(resp, req), req);
}

//获取所有的计算结果
void CSlaveClient::getAllState(GetAllStateResp& resp, const GetAllStateReq& req)
{
    if (!isInit())
    {
        setBase(resp.base, ErrCode::type::QUROOT_NOT_INIT);
        return;
    }

    CALL_WITH_SERVICE(m_client->getAllState(resp, req), req);
}

//执行任务
void CSlaveClient::run(RunCircuitResp& resp, const RunCircuitReq& req)
{
    if (!isInit())
    {
        setBase(resp.base, ErrCode::type::QUROOT_NOT_INIT);
        return;
    }

    CALL_WITH_SERVICE(m_client->run(resp, req), req);
}

//对部分量子比特应用量子傅立叶变换
void CSlaveClient::applyQFT(ApplyQFTResp& resp, const ApplyQFTReq& req)
{
    if (!isInit())
    {
        setBase(resp.base, ErrCode::type::QUROOT_NOT_INIT);
        return;
    }

    CALL_WITH_SERVICE(m_client->applyQFT(resp, req), req);
}

//对所有量子比特应用量子傅立叶变换
void CSlaveClient::applyFullQFT(ApplyFullQFTResp& resp, const ApplyFullQFTReq& req)
{
    if (!isInit())
    {
        setBase(resp.base, ErrCode::type::QUROOT_NOT_INIT);
        return;
    }

    CALL_WITH_SERVICE(m_client->applyFullQFT(resp, req), req);
}

//获取泡利算子乘积的期望值
void CSlaveClient::getExpecPauliProd(GetExpecPauliProdResp& resp, const GetExpecPauliProdReq& req)
{
    if (!isInit())
    {
        setBase(resp.base, ErrCode::type::QUROOT_NOT_INIT);
        return;
    }

    CALL_WITH_SERVICE(m_client->getExpecPauliProd(resp, req), req);
}

//获取泡利算子乘积之和的期望值
void CSlaveClient::getExpecPauliSum(GetExpecPauliSumResp& resp, const GetExpecPauliSumReq& req)
{
    if (!isInit())
    {
        setBase(resp.base, ErrCode::type::QUROOT_NOT_INIT);
        return;
    }

    CALL_WITH_SERVICE(m_client->getExpecPauliSum(resp, req), req);
}

//获取测量结果
void CSlaveClient::measureQubits(MeasureQubitsResp& resp, const MeasureQubitsReq& req)
{
    if (!isInit())
    {
        setBase(resp.base, ErrCode::type::QUROOT_NOT_INIT);
        return;
    }

    CALL_WITH_SERVICE(m_client->measureQubits(resp, req), req);
}

//注册一些自定义量子门，单次任务有效
void CSlaveClient::addCustomGateByMatrix(AddCustomGateByMatrixResp& resp, const AddCustomGateByMatrixReq& req)
{
    if (!isInit())
    {
        setBase(resp.base, ErrCode::type::QUROOT_NOT_INIT);
        return;
    }

    CALL_WITH_SERVICE(m_client->addCustomGateByMatrix(resp, req), req);
}

//添加量子门操作
void CSlaveClient::addSubCircuit(AddSubCircuitResp& resp, const AddSubCircuitReq& req)
{
    if (!isInit())
    {
        setBase(resp.base, ErrCode::type::QUROOT_NOT_INIT);
        return;
    }

    CALL_WITH_SERVICE(m_client->addSubCircuit(resp, req), req);
}

//追加量子比特到当前的量子电路
void CSlaveClient::appendQubits(AppendQubitsResp& resp, const AppendQubitsReq& req)
{
    if (!isInit())
    {
        setBase(resp.base, ErrCode::type::QUROOT_NOT_INIT);
        return;
    }

    CALL_WITH_SERVICE(m_client->appendQubits(resp, req), req);
}

//重置指定的qubits
void CSlaveClient::resetQubits(ResetQubitsResp& resp, const ResetQubitsReq& req)
{
    if (!isInit())
    {
        setBase(resp.base, ErrCode::type::QUROOT_NOT_INIT);
        return;
    }

    CALL_WITH_SERVICE(m_client->resetQubits(resp, req), req);
}

//获取当前量子状态向量
void CSlaveClient::getStateOfAllQubits(GetStateOfAllQubitsResp& resp, const GetStateOfAllQubitsReq& req)
{
    if (!isInit())
    {
        setBase(resp.base, ErrCode::type::QUROOT_NOT_INIT);
        return;
    }

    CALL_WITH_SERVICE(m_client->getStateOfAllQubits(resp, req), req);
}

//获取当前所有可能状态组合的概率
void CSlaveClient::getProbabilities(GetProbabilitiesResp& resp, const GetProbabilitiesReq& req)
{
    if (!isInit())
    {
        setBase(resp.base, ErrCode::type::QUROOT_NOT_INIT);
        return;
    }

    CALL_WITH_SERVICE(m_client->getProbabilities(resp, req), req);
}