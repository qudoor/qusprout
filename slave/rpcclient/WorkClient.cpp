#include <transport/TSocket.h>
#include <protocol/TBinaryProtocol.h>
#include <protocol/TMultiplexedProtocol.h>
#include "WorkClient.h"
#include "common/qulog.h"
#include "config/Config.h"
#include "common/Singleton.h"

CWorkClient::CWorkClient()
{

}

CWorkClient::~CWorkClient()
{
    close();
}

int CWorkClient::init(const std::string& addr, const int port)
{
    m_addr = addr;
    m_port = port;
    return reInit(false);
}

void CWorkClient::close()
{
    if (m_transport != nullptr)
    {
        m_transport->close();
        m_transport = nullptr;
    }
}

//重新初始化客户端
int CWorkClient::reInit(const bool isprint)
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

//qubit初始化
void CWorkClient::initQubits(InitQubitsResp& resp, const InitQubitsReq& req)
{
    std::string reqbuf = getPrint(req);
    LOG(INFO) << "request initQubits(req:" << reqbuf << ").";

    CALL_WITH_SERVICE(m_client->initQubits(resp, req), req);
    LOG(INFO) << "response initQubits(id:" << req.id << ",resp:" << getPrint(resp) << ").";
}

//设置振幅
void CWorkClient::setAmplitudes(SetAmplitudesResp& resp, const SetAmplitudesReq& req)
{
    CALL_WITH_SERVICE(m_client->setAmplitudes(resp, req), req);
}

//发送任务
void CWorkClient::sendCircuitCmd(SendCircuitCmdResp& resp, const SendCircuitCmdReq& req)
{
    CALL_WITH_SERVICE(m_client->sendCircuitCmd(resp, req), req);
}

//取消任务
void CWorkClient::cancelCmd(CancelCmdResp& resp, const CancelCmdReq& req)
{
    std::string reqbuf = getPrint(req);
    LOG(INFO) << "request cancelCmd(req:" << reqbuf << ").";

    CALL_WITH_SERVICE(m_client->cancelCmd(resp, req), req);
    LOG(INFO) << "response cancelCmd(id:" << req.id << ",resp:" << getPrint(resp) << ").";
}

//获取振幅
void CWorkClient::getProbAmp(GetProbAmpResp& resp, const GetProbAmpReq& req)
{
    CALL_WITH_SERVICE(m_client->getProbAmp(resp, req), req);
}

//获取当前qubit的概率
void CWorkClient::getProbOfOutcome(GetProbOfOutcomeResp& resp, const GetProbOfOutcomeReq& req)
{
    CALL_WITH_SERVICE(m_client->getProbOfOutcome(resp, req), req);
}

//获取所有qubit的概率
void CWorkClient::getProbOfAllOutcome(GetProbOfAllOutcomResp& resp, const GetProbOfAllOutcomReq& req)
{
    CALL_WITH_SERVICE(m_client->getProbOfAllOutcome(resp, req), req);
}

//获取所有的计算结果
void CWorkClient::getAllState(GetAllStateResp& resp, const GetAllStateReq& req)
{
    CALL_WITH_SERVICE(m_client->getAllState(resp, req), req);
}

//执行任务
void CWorkClient::run(RunCircuitResp& resp, const RunCircuitReq& req)
{
    CALL_WITH_SERVICE(m_client->run(resp, req), req);
}

//对部分量子比特应用量子傅立叶变换
void CWorkClient::applyQFT(ApplyQFTResp& resp, const ApplyQFTReq& req)
{
    CALL_WITH_SERVICE(m_client->applyQFT(resp, req), req);
}

//对所有量子比特应用量子傅立叶变换
void CWorkClient::applyFullQFT(ApplyFullQFTResp& resp, const ApplyFullQFTReq& req)
{
    CALL_WITH_SERVICE(m_client->applyFullQFT(resp, req), req);
}

//获取泡利算子乘积的期望值
void CWorkClient::getExpecPauliProd(GetExpecPauliProdResp& resp, const GetExpecPauliProdReq& req)
{
    CALL_WITH_SERVICE(m_client->getExpecPauliProd(resp, req), req);
}

//获取泡利算子乘积之和的期望值
void CWorkClient::getExpecPauliSum(GetExpecPauliSumResp& resp, const GetExpecPauliSumReq& req)
{
    CALL_WITH_SERVICE(m_client->getExpecPauliSum(resp, req), req);
}

//获取测量结果
void CWorkClient::measureQubits(MeasureQubitsResp& resp, const MeasureQubitsReq& req)
{
    CALL_WITH_SERVICE(m_client->measureQubits(resp, req), req);
}

//注册一些自定义量子门，单次任务有效
void CWorkClient::addCustomGateByMatrix(AddCustomGateByMatrixResp& resp, const AddCustomGateByMatrixReq& req)
{
    CALL_WITH_SERVICE(m_client->addCustomGateByMatrix(resp, req), req);
}

//添加量子门操作
void CWorkClient::addSubCircuit(AddSubCircuitResp& resp, const AddSubCircuitReq& req)
{
    CALL_WITH_SERVICE(m_client->addSubCircuit(resp, req), req);
}

//追加量子比特到当前的量子电路
void CWorkClient::appendQubits(AppendQubitsResp& resp, const AppendQubitsReq& req)
{
    CALL_WITH_SERVICE(m_client->appendQubits(resp, req), req);
}

//重置指定的qubits
void CWorkClient::resetQubits(ResetQubitsResp& resp, const ResetQubitsReq& req)
{
    CALL_WITH_SERVICE(m_client->resetQubits(resp, req), req);
}

//获取当前量子状态向量
void CWorkClient::getStateOfAllQubits(GetStateOfAllQubitsResp& resp, const GetStateOfAllQubitsReq& req)
{
    CALL_WITH_SERVICE(m_client->getStateOfAllQubits(resp, req), req);
}

//获取当前所有可能状态组合的概率
void CWorkClient::getProbabilities(GetProbabilitiesResp& resp, const GetProbabilitiesReq& req)
{
    CALL_WITH_SERVICE(m_client->getProbabilities(resp, req), req);
}
