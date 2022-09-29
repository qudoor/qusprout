#include <transport/TTransportException.h>
#include "common/qulog.h"
#include "common/Singleton.h"
#include "QuSproutServerHandle.h"
#include "taskmanager/TaskManager.h"

using namespace apache::thrift::transport;

CQuSproutServerHandler::CQuSproutServerHandler()
{

}

CQuSproutServerHandler::~CQuSproutServerHandler()
{

}

//qubit初始化
void CQuSproutServerHandler::initQubits(InitQubitsResp& resp, const InitQubitsReq& req)
{
    std::string reqbuf = getPrint<InitQubitsReq>(req);
    LOG(INFO) << "request initQubits(req:" << reqbuf << ").";

    SINGLETON(CTaskManager)->initQubits(resp, req);
    
    LOG(INFO) << "response initQubits(id:" << req.id << ",resp:" << getPrint<InitQubitsResp>(resp) << ").";
}

//发送任务
void CQuSproutServerHandler::sendCircuitCmd(SendCircuitCmdResp& resp, const SendCircuitCmdReq& req)
{
    std::string reqbuf = getPrint<SendCircuitCmdReq>(req);
    LOG(INFO) << "request sendCircuitCmd(req:" << reqbuf << ").";
    
    SINGLETON(CTaskManager)->sendCircuitCmd(resp, req);
}

//取消任务
void CQuSproutServerHandler::cancelCmd(CancelCmdResp& resp, const CancelCmdReq& req)
{
    std::string reqbuf = getPrint<CancelCmdReq>(req);
    LOG(INFO) << "request CancelCmdReq(req:" << reqbuf << ").";

    SINGLETON(CTaskManager)->cancelCmd(resp, req);

    LOG(INFO) << "response CancelCmdReq(id:" << req.id << ",resp:" << getPrint<CancelCmdResp>(resp) << ").";
}

//获取振幅
void CQuSproutServerHandler::getProbAmp(GetProbAmpResp& resp, const GetProbAmpReq& req)
{
    SINGLETON(CTaskManager)->getProbAmp(resp, req);
}

//获取当前qubit的概率
void CQuSproutServerHandler::getProbOfOutcome(GetProbOfOutcomeResp& resp, const GetProbOfOutcomeReq& req)
{
    SINGLETON(CTaskManager)->getProbOfOutcome(resp, req);
}

//获取所有qubit的概率
void CQuSproutServerHandler::getProbOfAllOutcome(GetProbOfAllOutcomResp& resp, const GetProbOfAllOutcomReq& req)
{
    SINGLETON(CTaskManager)->getProbOfAllOutcome(resp, req);
}

//获取所有的计算结果
void CQuSproutServerHandler::getAllState(GetAllStateResp& resp, const GetAllStateReq& req)
{
    SINGLETON(CTaskManager)->getAllState(resp, req);
}

//执行任务
void CQuSproutServerHandler::run(RunCircuitResp& resp, const RunCircuitReq& req)
{
    SINGLETON(CTaskManager)->run(resp, req);
}

//对部分量子比特应用量子傅立叶变换
void CQuSproutServerHandler::applyQFT(ApplyQFTResp& resp, const ApplyQFTReq& req)
{
    SINGLETON(CTaskManager)->applyQFT(resp, req);
}

//对所有量子比特应用量子傅立叶变换
void CQuSproutServerHandler::applyFullQFT(ApplyFullQFTResp& resp, const ApplyFullQFTReq& req)
{
    SINGLETON(CTaskManager)->applyFullQFT(resp, req);
}

//获取泡利算子乘积的期望值
void CQuSproutServerHandler::getExpecPauliProd(GetExpecPauliProdResp& resp, const GetExpecPauliProdReq& req)
{
    SINGLETON(CTaskManager)->getExpecPauliProd(resp, req);
}

//获取泡利算子乘积之和的期望值
void CQuSproutServerHandler::getExpecPauliSum(GetExpecPauliSumResp& resp, const GetExpecPauliSumReq& req)
{
    SINGLETON(CTaskManager)->getExpecPauliSum(resp, req);
}

//获取测量结果
void CQuSproutServerHandler::measureQubits(MeasureQubitsResp& resp, const MeasureQubitsReq& req)
{
    SINGLETON(CTaskManager)->measureQubits(resp, req);
}

//注册一些自定义量子门，单次任务有效
void CQuSproutServerHandler::addCustomGateByMatrix(AddCustomGateByMatrixResp& resp, const AddCustomGateByMatrixReq& req)
{
    SINGLETON(CTaskManager)->addCustomGateByMatrix(resp, req);
}

//添加量子门操作
void CQuSproutServerHandler::addSubCircuit(AddSubCircuitResp& resp, const AddSubCircuitReq& req)
{
    SINGLETON(CTaskManager)->addSubCircuit(resp, req);
}

//追加量子比特到当前的量子电路
void CQuSproutServerHandler::appendQubits(AppendQubitsResp& resp, const AppendQubitsReq& req)
{
    SINGLETON(CTaskManager)->appendQubits(resp, req);
}

//重置指定的qubits
void CQuSproutServerHandler::resetQubits(ResetQubitsResp& resp, const ResetQubitsReq& req)
{
    SINGLETON(CTaskManager)->resetQubits(resp, req);
}

//获取当前量子状态向量
void CQuSproutServerHandler::getStateOfAllQubits(GetStateOfAllQubitsResp& resp, const GetStateOfAllQubitsReq& req)
{
    SINGLETON(CTaskManager)->getStateOfAllQubits(resp, req);
}

//获取当前所有可能状态组合的概率
void CQuSproutServerHandler::getProbabilities(GetProbabilitiesResp& resp, const GetProbabilitiesReq& req)
{
    SINGLETON(CTaskManager)->getProbabilities(resp, req);
}
