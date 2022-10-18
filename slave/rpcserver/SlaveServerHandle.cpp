#include <transport/TTransportException.h>
#include "common/qulog.h"
#include "common/Singleton.h"
#include "SlaveServerHandle.h"
#include "taskmanager/TaskManager.h"

using namespace apache::thrift::transport;

CSlaveServerHandler::CSlaveServerHandler()
{

}

CSlaveServerHandler::~CSlaveServerHandler()
{

}

//qubit初始化
void CSlaveServerHandler::initQubits(InitQubitsResp& resp, const InitQubitsReq& req)
{
    SINGLETON(CTaskManager)->initQubits(resp, req);
}

//发送任务
void CSlaveServerHandler::sendCircuitCmd(SendCircuitCmdResp& resp, const SendCircuitCmdReq& req)
{
    SINGLETON(CTaskManager)->sendCircuitCmd(resp, req);
}

//取消任务
void CSlaveServerHandler::cancelCmd(CancelCmdResp& resp, const CancelCmdReq& req)
{
    SINGLETON(CTaskManager)->cancelCmd(resp, req);
}

//获取振幅
void CSlaveServerHandler::getProbAmp(GetProbAmpResp& resp, const GetProbAmpReq& req)
{
    SINGLETON(CTaskManager)->getProbAmp(resp, req);
}

//获取当前qubit的概率
void CSlaveServerHandler::getProbOfOutcome(GetProbOfOutcomeResp& resp, const GetProbOfOutcomeReq& req)
{
    SINGLETON(CTaskManager)->getProbOfOutcome(resp, req);
}

//获取所有qubit的概率
void CSlaveServerHandler::getProbOfAllOutcome(GetProbOfAllOutcomResp& resp, const GetProbOfAllOutcomReq& req)
{
    SINGLETON(CTaskManager)->getProbOfAllOutcome(resp, req);
}

//获取所有的计算结果
void CSlaveServerHandler::getAllState(GetAllStateResp& resp, const GetAllStateReq& req)
{
    SINGLETON(CTaskManager)->getAllState(resp, req);
}

//执行任务
void CSlaveServerHandler::run(RunCircuitResp& resp, const RunCircuitReq& req)
{
    SINGLETON(CTaskManager)->run(resp, req);
}

//对部分量子比特应用量子傅立叶变换
void CSlaveServerHandler::applyQFT(ApplyQFTResp& resp, const ApplyQFTReq& req)
{
    SINGLETON(CTaskManager)->applyQFT(resp, req);
}

//对所有量子比特应用量子傅立叶变换
void CSlaveServerHandler::applyFullQFT(ApplyFullQFTResp& resp, const ApplyFullQFTReq& req)
{
    SINGLETON(CTaskManager)->applyFullQFT(resp, req);
}

//获取泡利算子乘积的期望值
void CSlaveServerHandler::getExpecPauliProd(GetExpecPauliProdResp& resp, const GetExpecPauliProdReq& req)
{
    SINGLETON(CTaskManager)->getExpecPauliProd(resp, req);
}

//获取泡利算子乘积之和的期望值
void CSlaveServerHandler::getExpecPauliSum(GetExpecPauliSumResp& resp, const GetExpecPauliSumReq& req)
{
    SINGLETON(CTaskManager)->getExpecPauliSum(resp, req);
}

//获取测量结果
void CSlaveServerHandler::measureQubits(MeasureQubitsResp& resp, const MeasureQubitsReq& req)
{
    SINGLETON(CTaskManager)->measureQubits(resp, req);
}

//注册一些自定义量子门，单次任务有效
void CSlaveServerHandler::addCustomGateByMatrix(AddCustomGateByMatrixResp& resp, const AddCustomGateByMatrixReq& req)
{
    SINGLETON(CTaskManager)->addCustomGateByMatrix(resp, req);
}

//添加量子门操作
void CSlaveServerHandler::addSubCircuit(AddSubCircuitResp& resp, const AddSubCircuitReq& req)
{
    SINGLETON(CTaskManager)->addSubCircuit(resp, req);
}

//追加量子比特到当前的量子电路
void CSlaveServerHandler::appendQubits(AppendQubitsResp& resp, const AppendQubitsReq& req)
{
    SINGLETON(CTaskManager)->appendQubits(resp, req);
}

//重置指定的qubits
void CSlaveServerHandler::resetQubits(ResetQubitsResp& resp, const ResetQubitsReq& req)
{
    SINGLETON(CTaskManager)->resetQubits(resp, req);
}

//获取当前量子状态向量
void CSlaveServerHandler::getStateOfAllQubits(GetStateOfAllQubitsResp& resp, const GetStateOfAllQubitsReq& req)
{
    SINGLETON(CTaskManager)->getStateOfAllQubits(resp, req);
}

//获取当前所有可能状态组合的概率
void CSlaveServerHandler::getProbabilities(GetProbabilitiesResp& resp, const GetProbabilitiesReq& req)
{
    SINGLETON(CTaskManager)->getProbabilities(resp, req);
}

//获取任务状态
void CSlaveServerHandler::getTaskInfo(GetTaskInfoResp& resp, const GetTaskInfoReq& req)
{
    setBase(resp.base, ErrCode::type::COM_INVALID_PARAM);
}
