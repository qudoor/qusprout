/****************************************************************************
*
* This code is licensed under the Apache License, Version 2.0. You may
* obtain a copy of this license in the LICENSE.txt file in the root directory
* of this source tree or at http://www.apache.org/licenses/LICENSE-2.0.
*
* Any modifications or derivative works of this code must retain this
* copyright notice, and modified files need to carry a notice indicating
* that they have been altered from the originals.
*
****************************************************************************/

#include <transport/TSocket.h>
#include <protocol/TBinaryProtocol.h>
#include <protocol/TMultiplexedProtocol.h>
#include "QuWorkClient.h"
#include "common/qulog.h"
#include "config/Config.h"
#include "common/Singleton.h"

CQuWorkClient::CQuWorkClient()
{

}

CQuWorkClient::~CQuWorkClient()
{
    close();
}

bool CQuWorkClient::isInit()
{
    return m_isInit;
}

int CQuWorkClient::init(const std::string& addr, const int port)
{
    m_addr = addr;
    m_port = port;
    return reInit();
}

void CQuWorkClient::close()
{
    if (m_transport != nullptr)
    {
        m_transport->close();
        m_transport = nullptr;
    }
}

//重新初始化客户端
int CQuWorkClient::reInit()
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
        //LOG(ERROR) << "init exception(err:" << e.what() << ").";
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
void CQuWorkClient::initQubits(InitQubitsResp& resp, const InitQubitsReq& req)
{
    LOG(INFO) << "request initQubits(req:" << getPrint(req) << ").";

    if (ExecCmdType::ExecTypeCpuMpi == req.exec_type)
    {
        setBase(resp.base, ErrCode::type::COM_SUCCESS);
    }
    else
    {
        CALL_WITH_SERVICE(m_client->initQubits(resp, req), req);
    }
    if (resp.base.code == ErrCode::type::COM_SUCCESS)
    {
        m_isInit = true;
    }
    
    LOG(INFO) << "response initQubits(id:" << req.id << ",resp:" << getPrint(resp) << ").";
}

//发送任务
void CQuWorkClient::sendCircuitCmd(SendCircuitCmdResp& resp, const SendCircuitCmdReq& req)
{
    if (!isInit())
    {
        setBase(resp.base, ErrCode::type::COM_NOT_INIT, "not connected to work process.");
        return;
    }

    CALL_WITH_SERVICE(m_client->sendCircuitCmd(resp, req), req);
}

//取消任务
void CQuWorkClient::cancelCmd(CancelCmdResp& resp, const CancelCmdReq& req)
{
    LOG(INFO) << "request cancelCmd(req:" << getPrint(req) << ").";

    if (!isInit())
    {
        setBase(resp.base, ErrCode::type::COM_NOT_INIT, "not connected to work process.");
        return;
    }

    CALL_WITH_SERVICE(m_client->cancelCmd(resp, req), req);
    LOG(INFO) << "response cancelCmd(id:" << req.id << ",resp:" << getPrint(resp) << ").";
}

//获取振幅
void CQuWorkClient::getProbAmp(GetProbAmpResp& resp, const GetProbAmpReq& req)
{
    if (!isInit())
    {
        setBase(resp.base, ErrCode::type::COM_NOT_INIT, "not connected to work process.");
        return;
    }

    CALL_WITH_SERVICE(m_client->getProbAmp(resp, req), req);
}

//获取所有qubit的概率
void CQuWorkClient::getProbOfAllOutcome(GetProbOfAllOutcomResp& resp, const GetProbOfAllOutcomReq& req)
{
    if (!isInit())
    {
        setBase(resp.base, ErrCode::type::COM_NOT_INIT, "not connected to work process.");
        return;
    }

    CALL_WITH_SERVICE(m_client->getProbOfAllOutcome(resp, req), req);
}

//获取所有的计算结果
void CQuWorkClient::getAllState(GetAllStateResp& resp, const GetAllStateReq& req)
{
    if (!isInit())
    {
        setBase(resp.base, ErrCode::type::COM_NOT_INIT, "not connected to work process.");
        return;
    }

    CALL_WITH_SERVICE(m_client->getAllState(resp, req), req);
}

//执行任务
void CQuWorkClient::run(RunCircuitResp& resp, const RunCircuitReq& req)
{
    if (!isInit())
    {
        setBase(resp.base, ErrCode::type::COM_NOT_INIT, "not connected to work process.");
        return;
    }

    CALL_WITH_SERVICE(m_client->run(resp, req), req);
}

//获取泡利算子乘积的期望值
void CQuWorkClient::getExpecPauliProd(GetExpecPauliProdResp& resp, const GetExpecPauliProdReq& req)
{
    if (!isInit())
    {
        setBase(resp.base, ErrCode::type::COM_NOT_INIT, "not connected to work process.");
        return;
    }

    CALL_WITH_SERVICE(m_client->getExpecPauliProd(resp, req), req);
}

//获取泡利算子乘积之和的期望值
void CQuWorkClient::getExpecPauliSum(GetExpecPauliSumResp& resp, const GetExpecPauliSumReq& req)
{
    if (!isInit())
    {
        setBase(resp.base, ErrCode::type::COM_NOT_INIT, "not connected to work process.");
        return;
    }

    CALL_WITH_SERVICE(m_client->getExpecPauliSum(resp, req), req);
}

//获取测量结果
void CQuWorkClient::measureQubits(MeasureQubitsResp& resp, const MeasureQubitsReq& req)
{
    if (!isInit())
    {
        setBase(resp.base, ErrCode::type::COM_NOT_INIT, "not connected to work process.");
        return;
    }

    CALL_WITH_SERVICE(m_client->measureQubits(resp, req), req);
}

//注册一些自定义量子门，单次任务有效
void CQuWorkClient::addCustomGateByMatrix(AddCustomGateByMatrixResp& resp, const AddCustomGateByMatrixReq& req)
{
    if (!isInit())
    {
        setBase(resp.base, ErrCode::type::COM_NOT_INIT, "not connected to work process.");
        return;
    }

    CALL_WITH_SERVICE(m_client->addCustomGateByMatrix(resp, req), req);
}

//添加量子门操作
void CQuWorkClient::addSubCircuit(AddSubCircuitResp& resp, const AddSubCircuitReq& req)
{
    if (!isInit())
    {
        setBase(resp.base, ErrCode::type::COM_NOT_INIT, "not connected to work process.");
        return;
    }

    CALL_WITH_SERVICE(m_client->addSubCircuit(resp, req), req);
}

//追加量子比特到当前的量子电路
void CQuWorkClient::appendQubits(AppendQubitsResp& resp, const AppendQubitsReq& req)
{
    if (!isInit())
    {
        setBase(resp.base, ErrCode::type::COM_NOT_INIT, "not connected to work process.");
        return;
    }

    CALL_WITH_SERVICE(m_client->appendQubits(resp, req), req);
}

//重置指定的qubits
void CQuWorkClient::resetQubits(ResetQubitsResp& resp, const ResetQubitsReq& req)
{
    if (!isInit())
    {
        setBase(resp.base, ErrCode::type::COM_NOT_INIT, "not connected to work process.");
        return;
    }

    CALL_WITH_SERVICE(m_client->resetQubits(resp, req), req);
}

//获取当前量子状态向量
void CQuWorkClient::getStateOfAllQubits(GetStateOfAllQubitsResp& resp, const GetStateOfAllQubitsReq& req)
{
    if (!isInit())
    {
        setBase(resp.base, ErrCode::type::COM_NOT_INIT, "not connected to work process.");
        return;
    }

    CALL_WITH_SERVICE(m_client->getStateOfAllQubits(resp, req), req);
}

//获取当前所有可能状态组合的概率
void CQuWorkClient::getProbabilities(GetProbabilitiesResp& resp, const GetProbabilitiesReq& req)
{
    if (!isInit())
    {
        setBase(resp.base, ErrCode::type::COM_NOT_INIT, "not connected to work process.");
        return;
    }

    CALL_WITH_SERVICE(m_client->getProbabilities(resp, req), req);
}