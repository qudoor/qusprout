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

#include <transport/TTransportException.h>
#include "common/qulog.h"
#include "common/Singleton.h"
#include "QuSproutServerHandle.h"
#include "taskmanager/TaskManager.h"
#include "metrics/metrics.h"

using namespace apache::thrift::transport;

const std::string CMD_STR_INITQUBITS = "initQubits";
const std::string CMD_STR_SENDCIRCUITCMD = "sendCircuitCmd";
const std::string CMD_STR_CANCELCMD = "cancelCmd";
const std::string CMD_STR_GETPROBAMP = "getProbAmp";
const std::string CMD_STR_GETPROBOFOUTCOME = "getProbOfOutcome";
const std::string CMD_STR_GETPROBOFALLOUTCOME = "getProbOfAllOutcome";
const std::string CMD_STR_GETALLSTATE = "getAllState";
const std::string CMD_STR_RUN = "run";
const std::string CMD_STR_APPLYQFT = "applyQFT";
const std::string CMD_STR_APPLYFULLQFT = "applyFullQFT";
const std::string CMD_STR_GETEXPECPAULIPROD = "getExpecPauliProd";
const std::string CMD_STR_GETEXPECPAULISUM = "getExpecPauliSum";
const std::string CMD_STR_MEASUREQUBITS = "measureQubits";
const std::string CMD_STR_ADDCUSTOMGATEBYMATRIX = "addCustomGateByMatrix";
const std::string CMD_STR_ADDSUBCIRCUIT = "addSubCircuit";
const std::string CMD_STR_APPENDQUBITS = "appendQubits";
const std::string CMD_STR_RESETQUBITS = "resetQubits";
const std::string CMD_STR_GETSTATEOFALLQUBITS = "getStateOfAllQubits";
const std::string CMD_STR_GETPROBABILITIES = "getProbabilities";
const std::string CMD_STR_GETTASKINFO = "getTaskInfo";

CQuSproutServerHandler::CQuSproutServerHandler()
{

}

CQuSproutServerHandler::~CQuSproutServerHandler()
{

}

//qubit初始化
void CQuSproutServerHandler::initQubits(InitQubitsResp& resp, const InitQubitsReq& req)
{
    CMetricsGuard metrics(CMD_STR_INITQUBITS, (int)resp.base.code);

    LOG(INFO) << "request initQubits(req:" << getPrint(req) << ").";

    SINGLETON(CTaskManager)->initQubits(resp, req);
    
    LOG(INFO) << "response initQubits(id:" << req.id << ",resp:" << getPrint(resp) << ").";
}

//发送任务
void CQuSproutServerHandler::sendCircuitCmd(SendCircuitCmdResp& resp, const SendCircuitCmdReq& req)
{
    CMetricsGuard metrics(CMD_STR_SENDCIRCUITCMD, (int)resp.base.code);

    LOG(INFO) << "request sendCircuitCmd(req:" << getPrint(req) << ").";
    
    SINGLETON(CTaskManager)->sendCircuitCmd(resp, req);
}

//取消任务
void CQuSproutServerHandler::cancelCmd(CancelCmdResp& resp, const CancelCmdReq& req)
{
    CMetricsGuard metrics(CMD_STR_CANCELCMD, (int)resp.base.code);

    LOG(INFO) << "request CancelCmdReq(req:" << getPrint(req) << ").";

    SINGLETON(CTaskManager)->cancelCmd(resp, req);

    LOG(INFO) << "response CancelCmdReq(id:" << req.id << ",resp:" << getPrint(resp) << ").";
}

//获取振幅
void CQuSproutServerHandler::getProbAmp(GetProbAmpResp& resp, const GetProbAmpReq& req)
{
    CMetricsGuard metrics(CMD_STR_GETPROBAMP, (int)resp.base.code);

    SINGLETON(CTaskManager)->getProbAmp(resp, req);
}

//获取当前qubit的概率
void CQuSproutServerHandler::getProbOfOutcome(GetProbOfOutcomeResp& resp, const GetProbOfOutcomeReq& req)
{
    CMetricsGuard metrics(CMD_STR_GETPROBOFOUTCOME, (int)resp.base.code);

    SINGLETON(CTaskManager)->getProbOfOutcome(resp, req);
}

//获取所有qubit的概率
void CQuSproutServerHandler::getProbOfAllOutcome(GetProbOfAllOutcomResp& resp, const GetProbOfAllOutcomReq& req)
{
    CMetricsGuard metrics(CMD_STR_GETPROBOFALLOUTCOME, (int)resp.base.code);

    SINGLETON(CTaskManager)->getProbOfAllOutcome(resp, req);
}

//获取所有的计算结果
void CQuSproutServerHandler::getAllState(GetAllStateResp& resp, const GetAllStateReq& req)
{
    CMetricsGuard metrics(CMD_STR_GETALLSTATE, (int)resp.base.code);

    SINGLETON(CTaskManager)->getAllState(resp, req);
}

//执行任务
void CQuSproutServerHandler::run(RunCircuitResp& resp, const RunCircuitReq& req)
{
    CMetricsGuard metrics(CMD_STR_RUN, (int)resp.base.code);

    SINGLETON(CTaskManager)->run(resp, req);
}

//对部分量子比特应用量子傅立叶变换
void CQuSproutServerHandler::applyQFT(ApplyQFTResp& resp, const ApplyQFTReq& req)
{
    
    CMetricsGuard metrics(CMD_STR_APPLYQFT, (int)resp.base.code);

    SINGLETON(CTaskManager)->applyQFT(resp, req);
}

//对所有量子比特应用量子傅立叶变换
void CQuSproutServerHandler::applyFullQFT(ApplyFullQFTResp& resp, const ApplyFullQFTReq& req)
{
    CMetricsGuard metrics(CMD_STR_APPLYFULLQFT, (int)resp.base.code);
    
    SINGLETON(CTaskManager)->applyFullQFT(resp, req);
}

//获取泡利算子乘积的期望值
void CQuSproutServerHandler::getExpecPauliProd(GetExpecPauliProdResp& resp, const GetExpecPauliProdReq& req)
{
    CMetricsGuard metrics(CMD_STR_GETEXPECPAULIPROD, (int)resp.base.code);

    SINGLETON(CTaskManager)->getExpecPauliProd(resp, req);
}

//获取泡利算子乘积之和的期望值
void CQuSproutServerHandler::getExpecPauliSum(GetExpecPauliSumResp& resp, const GetExpecPauliSumReq& req)
{
    CMetricsGuard metrics(CMD_STR_GETEXPECPAULISUM, (int)resp.base.code);

    SINGLETON(CTaskManager)->getExpecPauliSum(resp, req);
}

//获取测量结果
void CQuSproutServerHandler::measureQubits(MeasureQubitsResp& resp, const MeasureQubitsReq& req)
{
    CMetricsGuard metrics(CMD_STR_MEASUREQUBITS, (int)resp.base.code);

    SINGLETON(CTaskManager)->measureQubits(resp, req);
}

//注册一些自定义量子门，单次任务有效
void CQuSproutServerHandler::addCustomGateByMatrix(AddCustomGateByMatrixResp& resp, const AddCustomGateByMatrixReq& req)
{
    CMetricsGuard metrics(CMD_STR_ADDCUSTOMGATEBYMATRIX, (int)resp.base.code);

    SINGLETON(CTaskManager)->addCustomGateByMatrix(resp, req);
}

//添加量子门操作
void CQuSproutServerHandler::addSubCircuit(AddSubCircuitResp& resp, const AddSubCircuitReq& req)
{
    CMetricsGuard metrics(CMD_STR_ADDSUBCIRCUIT, (int)resp.base.code);

    SINGLETON(CTaskManager)->addSubCircuit(resp, req);
}

//追加量子比特到当前的量子电路
void CQuSproutServerHandler::appendQubits(AppendQubitsResp& resp, const AppendQubitsReq& req)
{
    CMetricsGuard metrics(CMD_STR_APPENDQUBITS, (int)resp.base.code);

    SINGLETON(CTaskManager)->appendQubits(resp, req);
}

//重置指定的qubits
void CQuSproutServerHandler::resetQubits(ResetQubitsResp& resp, const ResetQubitsReq& req)
{
    CMetricsGuard metrics(CMD_STR_RESETQUBITS, (int)resp.base.code);

    SINGLETON(CTaskManager)->resetQubits(resp, req);
}

//获取当前量子状态向量
void CQuSproutServerHandler::getStateOfAllQubits(GetStateOfAllQubitsResp& resp, const GetStateOfAllQubitsReq& req)
{
    CMetricsGuard metrics(CMD_STR_GETSTATEOFALLQUBITS, (int)resp.base.code);

    SINGLETON(CTaskManager)->getStateOfAllQubits(resp, req);
}

//获取当前所有可能状态组合的概率
void CQuSproutServerHandler::getProbabilities(GetProbabilitiesResp& resp, const GetProbabilitiesReq& req)
{
    CMetricsGuard metrics(CMD_STR_GETPROBABILITIES, (int)resp.base.code);

    SINGLETON(CTaskManager)->getProbabilities(resp, req);
}

//获取任务状态
void CQuSproutServerHandler::getTaskInfo(GetTaskInfoResp& resp, const GetTaskInfoReq& req)
{
    CMetricsGuard metrics(CMD_STR_GETTASKINFO, (int)resp.base.code);

    SINGLETON(CTaskManager)->getTaskInfo(resp, req);
}