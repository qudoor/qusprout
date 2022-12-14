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
#include "random/RandomCard.h"

using namespace apache::thrift::transport;

const std::string CMD_STR_INITQUBITS = "initQubits";
const std::string CMD_STR_SENDCIRCUITCMD = "sendCircuitCmd";
const std::string CMD_STR_CANCELCMD = "cancelCmd";
const std::string CMD_STR_GETPROBAMP = "getProbAmp";
const std::string CMD_STR_GETPROBOFALLOUTCOME = "getProbOfAllOutcome";
const std::string CMD_STR_GETALLSTATE = "getAllState";
const std::string CMD_STR_RUN = "run";
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
const std::string CMD_STR_GETRANDOMCARDINFO = "getRandomCardInfo";
const std::string CMD_STR_SETRANDOMCARD = "setRandomCard";
const std::string CMD_STR_GETRANDOM = "getRandom";

CQuSproutServerHandler::CQuSproutServerHandler()
{

}

CQuSproutServerHandler::~CQuSproutServerHandler()
{

}

//qubit?????????
void CQuSproutServerHandler::initQubits(InitQubitsResp& resp, const InitQubitsReq& req)
{
    CMetricsGuard metrics(CMD_STR_INITQUBITS, (int)resp.base.code);

    LOG(INFO) << "request initQubits(req:" << getPrint(req) << ").";

    SINGLETON(CTaskManager)->initQubits(resp, req);
    
    LOG(INFO) << "response initQubits(id:" << req.id << ",resp:" << getPrint(resp) << ").";
}

//????????????
void CQuSproutServerHandler::sendCircuitCmd(SendCircuitCmdResp& resp, const SendCircuitCmdReq& req)
{
    CMetricsGuard metrics(CMD_STR_SENDCIRCUITCMD, (int)resp.base.code);

    LOG(INFO) << "request sendCircuitCmd(req:" << getPrint(req) << ").";
    
    SINGLETON(CTaskManager)->sendCircuitCmd(resp, req);
}

//????????????
void CQuSproutServerHandler::cancelCmd(CancelCmdResp& resp, const CancelCmdReq& req)
{
    CMetricsGuard metrics(CMD_STR_CANCELCMD, (int)resp.base.code);

    LOG(INFO) << "request CancelCmdReq(req:" << getPrint(req) << ").";

    SINGLETON(CTaskManager)->cancelCmd(resp, req);

    LOG(INFO) << "response CancelCmdReq(id:" << req.id << ",resp:" << getPrint(resp) << ").";
}

//????????????
void CQuSproutServerHandler::getProbAmp(GetProbAmpResp& resp, const GetProbAmpReq& req)
{
    CMetricsGuard metrics(CMD_STR_GETPROBAMP, (int)resp.base.code);

    SINGLETON(CTaskManager)->getProbAmp(resp, req);
}

//????????????qubit?????????
void CQuSproutServerHandler::getProbOfAllOutcome(GetProbOfAllOutcomResp& resp, const GetProbOfAllOutcomReq& req)
{
    CMetricsGuard metrics(CMD_STR_GETPROBOFALLOUTCOME, (int)resp.base.code);

    SINGLETON(CTaskManager)->getProbOfAllOutcome(resp, req);
}

//???????????????????????????
void CQuSproutServerHandler::getAllState(GetAllStateResp& resp, const GetAllStateReq& req)
{
    CMetricsGuard metrics(CMD_STR_GETALLSTATE, (int)resp.base.code);

    SINGLETON(CTaskManager)->getAllState(resp, req);
}

//????????????
void CQuSproutServerHandler::run(RunCircuitResp& resp, const RunCircuitReq& req)
{
    CMetricsGuard metrics(CMD_STR_RUN, (int)resp.base.code);

    SINGLETON(CTaskManager)->run(resp, req);
}

//????????????????????????????????????
void CQuSproutServerHandler::getExpecPauliProd(GetExpecPauliProdResp& resp, const GetExpecPauliProdReq& req)
{
    CMetricsGuard metrics(CMD_STR_GETEXPECPAULIPROD, (int)resp.base.code);

    SINGLETON(CTaskManager)->getExpecPauliProd(resp, req);
}

//??????????????????????????????????????????
void CQuSproutServerHandler::getExpecPauliSum(GetExpecPauliSumResp& resp, const GetExpecPauliSumReq& req)
{
    CMetricsGuard metrics(CMD_STR_GETEXPECPAULISUM, (int)resp.base.code);

    SINGLETON(CTaskManager)->getExpecPauliSum(resp, req);
}

//??????????????????
void CQuSproutServerHandler::measureQubits(MeasureQubitsResp& resp, const MeasureQubitsReq& req)
{
    CMetricsGuard metrics(CMD_STR_MEASUREQUBITS, (int)resp.base.code);

    SINGLETON(CTaskManager)->measureQubits(resp, req);
}

//???????????????????????????????????????????????????
void CQuSproutServerHandler::addCustomGateByMatrix(AddCustomGateByMatrixResp& resp, const AddCustomGateByMatrixReq& req)
{
    CMetricsGuard metrics(CMD_STR_ADDCUSTOMGATEBYMATRIX, (int)resp.base.code);

    SINGLETON(CTaskManager)->addCustomGateByMatrix(resp, req);
}

//?????????????????????
void CQuSproutServerHandler::addSubCircuit(AddSubCircuitResp& resp, const AddSubCircuitReq& req)
{
    CMetricsGuard metrics(CMD_STR_ADDSUBCIRCUIT, (int)resp.base.code);

    SINGLETON(CTaskManager)->addSubCircuit(resp, req);
}

//??????????????????????????????????????????
void CQuSproutServerHandler::appendQubits(AppendQubitsResp& resp, const AppendQubitsReq& req)
{
    CMetricsGuard metrics(CMD_STR_APPENDQUBITS, (int)resp.base.code);

    SINGLETON(CTaskManager)->appendQubits(resp, req);
}

//???????????????qubits
void CQuSproutServerHandler::resetQubits(ResetQubitsResp& resp, const ResetQubitsReq& req)
{
    CMetricsGuard metrics(CMD_STR_RESETQUBITS, (int)resp.base.code);

    SINGLETON(CTaskManager)->resetQubits(resp, req);
}

//??????????????????????????????
void CQuSproutServerHandler::getStateOfAllQubits(GetStateOfAllQubitsResp& resp, const GetStateOfAllQubitsReq& req)
{
    CMetricsGuard metrics(CMD_STR_GETSTATEOFALLQUBITS, (int)resp.base.code);

    SINGLETON(CTaskManager)->getStateOfAllQubits(resp, req);
}

//?????????????????????????????????????????????
void CQuSproutServerHandler::getProbabilities(GetProbabilitiesResp& resp, const GetProbabilitiesReq& req)
{
    CMetricsGuard metrics(CMD_STR_GETPROBABILITIES, (int)resp.base.code);

    SINGLETON(CTaskManager)->getProbabilities(resp, req);
}

//??????????????????
void CQuSproutServerHandler::getTaskInfo(GetTaskInfoResp& resp, const GetTaskInfoReq& req)
{
    CMetricsGuard metrics(CMD_STR_GETTASKINFO, (int)resp.base.code);

    SINGLETON(CTaskManager)->getTaskInfo(resp, req);
}

//???????????????????????????
void CQuSproutServerHandler::getRandomCardInfo(GetRandomCardInfoResp& resp, const GetRandomCardInfoReq& req)
{
    CMetricsGuard metrics(CMD_STR_GETRANDOMCARDINFO, (int)resp.base.code);

    SINGLETON(CRandomCardManager)->getRandomCardInfo(resp, req);
}

//??????????????????
void CQuSproutServerHandler::setRandomCard(SetRandomCardResp& resp, const SetRandomCardReq& req)
{
    CMetricsGuard metrics(CMD_STR_SETRANDOMCARD, (int)resp.base.code);

    SINGLETON(CRandomCardManager)->setRandomCard(resp, req);
}

//???????????????
void CQuSproutServerHandler::getRandom(GetRandomResp& resp, const GetRandomReq& req)
{
    CMetricsGuard metrics(CMD_STR_GETRANDOM, (int)resp.base.code);

    SINGLETON(CRandomCardManager)->getRandom(resp, req);
}