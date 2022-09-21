#include <transport/TTransportException.h>
#include "common/qulog.h"
#include "common/Singleton.h"
#include "SlaveServerHandle.h"
#include "taskmanager/TaskManager.h"
#include "statistics/Statistics.h"

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
    CStatisticsTimeAndCount countsis(req.exec_type, Tag_Iinterface_initQubits, req.qubits);

    SINGLETON(CTaskManager)->initQubits(resp, req);

    CStatisticsCodeCount codesis(req.exec_type, Tag_Iinterface_initQubits, req.qubits, resp.base.code);
}

//设置振幅
void CSlaveServerHandler::setAmplitudes(SetAmplitudesResp& resp, const SetAmplitudesReq& req)
{
    InitQubitsReq initinfo;
    initinfo.__set_exec_type(ExecCmdType::ExecTypeDefault);
    CStatisticsTimeAndCount countsis(initinfo.exec_type, Tag_Iinterface_setAmplitudes);

    SINGLETON(CTaskManager)->setAmplitudes(resp, req, initinfo);
    countsis.setExecType(initinfo.exec_type);
    countsis.setQubitNum(initinfo.qubits);

    CStatisticsCodeCount codesis(initinfo.exec_type, Tag_Iinterface_setAmplitudes, initinfo.qubits, resp.base.code);
}

//发送任务
void CSlaveServerHandler::sendCircuitCmd(SendCircuitCmdResp& resp, const SendCircuitCmdReq& req)
{
    InitQubitsReq initinfo;
    initinfo.__set_exec_type(ExecCmdType::ExecTypeDefault);
    CStatisticsTimeAndCount countsis(initinfo.exec_type, Tag_Iinterface_sendCircuitCmd);

    SINGLETON(CTaskManager)->sendCircuitCmd(resp, req, initinfo);
    countsis.setExecType(initinfo.exec_type);
    countsis.setQubitNum(initinfo.qubits);

    CStatisticsCodeCount codesis(initinfo.exec_type, Tag_Iinterface_sendCircuitCmd, initinfo.qubits, resp.base.code);
}

//取消任务
void CSlaveServerHandler::cancelCmd(CancelCmdResp& resp, const CancelCmdReq& req)
{
    InitQubitsReq initinfo;
    initinfo.__set_exec_type(ExecCmdType::ExecTypeDefault);
    CStatisticsTimeAndCount countsis(initinfo.exec_type, Tag_Iinterface_cancelCmd);

    SINGLETON(CTaskManager)->cancelCmd(resp, req, initinfo);
    countsis.setExecType(initinfo.exec_type);
    countsis.setQubitNum(initinfo.qubits);

    CStatisticsCodeCount codesis(initinfo.exec_type, Tag_Iinterface_cancelCmd, initinfo.qubits, resp.base.code);
}

//获取振幅
void CSlaveServerHandler::getProbAmp(GetProbAmpResp& resp, const GetProbAmpReq& req)
{
    InitQubitsReq initinfo;
    initinfo.__set_exec_type(ExecCmdType::ExecTypeDefault);
    CStatisticsTimeAndCount countsis(initinfo.exec_type, Tag_Iinterface_getProbAmp);

    SINGLETON(CTaskManager)->getProbAmp(resp, req, initinfo);
    countsis.setExecType(initinfo.exec_type);
    countsis.setQubitNum(initinfo.qubits);

    CStatisticsCodeCount codesis(initinfo.exec_type, Tag_Iinterface_getProbAmp, initinfo.qubits, resp.base.code);
}

//获取当前qubit的概率
void CSlaveServerHandler::getProbOfOutcome(GetProbOfOutcomeResp& resp, const GetProbOfOutcomeReq& req)
{
    InitQubitsReq initinfo;
    initinfo.__set_exec_type(ExecCmdType::ExecTypeDefault);
    CStatisticsTimeAndCount countsis(initinfo.exec_type, Tag_Iinterface_getProbOfOutcome);

    SINGLETON(CTaskManager)->getProbOfOutcome(resp, req, initinfo);
    countsis.setExecType(initinfo.exec_type);
    countsis.setQubitNum(initinfo.qubits);

    CStatisticsCodeCount codesis(initinfo.exec_type, Tag_Iinterface_getProbOfOutcome, initinfo.qubits, resp.base.code);
}

//获取所有qubit的概率
void CSlaveServerHandler::getProbOfAllOutcome(GetProbOfAllOutcomResp& resp, const GetProbOfAllOutcomReq& req)
{
    InitQubitsReq initinfo;
    initinfo.__set_exec_type(ExecCmdType::ExecTypeDefault);
    CStatisticsTimeAndCount countsis(initinfo.exec_type, Tag_Iinterface_getProbOfAllOutcome);

    SINGLETON(CTaskManager)->getProbOfAllOutcome(resp, req, initinfo);
    countsis.setExecType(initinfo.exec_type);
    countsis.setQubitNum(initinfo.qubits);

    CStatisticsCodeCount codesis(initinfo.exec_type, Tag_Iinterface_getProbOfAllOutcome, initinfo.qubits, resp.base.code);
}

//获取所有的计算结果
void CSlaveServerHandler::getAllState(GetAllStateResp& resp, const GetAllStateReq& req)
{
    InitQubitsReq initinfo;
    initinfo.__set_exec_type(ExecCmdType::ExecTypeDefault);
    CStatisticsTimeAndCount countsis(initinfo.exec_type, Tag_Iinterface_getAllState);

    SINGLETON(CTaskManager)->getAllState(resp, req, initinfo);
    countsis.setExecType(initinfo.exec_type);
    countsis.setQubitNum(initinfo.qubits);

    CStatisticsCodeCount codesis(initinfo.exec_type, Tag_Iinterface_getAllState, initinfo.qubits, resp.base.code);
}

//执行任务
void CSlaveServerHandler::run(RunCircuitResp& resp, const RunCircuitReq& req)
{
    InitQubitsReq initinfo;
    initinfo.__set_exec_type(ExecCmdType::ExecTypeDefault);
    CStatisticsTimeAndCount countsis(initinfo.exec_type, Tag_Iinterface_run);

    SINGLETON(CTaskManager)->run(resp, req, initinfo);
    countsis.setExecType(initinfo.exec_type);
    countsis.setQubitNum(initinfo.qubits);

    CStatisticsCodeCount codesis(initinfo.exec_type, Tag_Iinterface_run, initinfo.qubits, resp.base.code);
}

//对部分量子比特应用量子傅立叶变换
void CSlaveServerHandler::applyQFT(ApplyQFTResp& resp, const ApplyQFTReq& req)
{
    InitQubitsReq initinfo;
    initinfo.__set_exec_type(ExecCmdType::ExecTypeDefault);
    CStatisticsTimeAndCount countsis(initinfo.exec_type, Tag_Iinterface_applyQFT);

    SINGLETON(CTaskManager)->applyQFT(resp, req, initinfo);
    countsis.setExecType(initinfo.exec_type);
    countsis.setQubitNum(initinfo.qubits);

    CStatisticsCodeCount codesis(initinfo.exec_type, Tag_Iinterface_applyQFT, initinfo.qubits, resp.base.code);
}

//对所有量子比特应用量子傅立叶变换
void CSlaveServerHandler::applyFullQFT(ApplyFullQFTResp& resp, const ApplyFullQFTReq& req)
{
    InitQubitsReq initinfo;
    initinfo.__set_exec_type(ExecCmdType::ExecTypeDefault);
    CStatisticsTimeAndCount countsis(initinfo.exec_type, Tag_Iinterface_applyFullQFT);

    SINGLETON(CTaskManager)->applyFullQFT(resp, req, initinfo);
    countsis.setExecType(initinfo.exec_type);
    countsis.setQubitNum(initinfo.qubits);

    CStatisticsCodeCount codesis(initinfo.exec_type, Tag_Iinterface_applyFullQFT, initinfo.qubits, resp.base.code);
}

//获取泡利算子乘积的期望值
void CSlaveServerHandler::getExpecPauliProd(GetExpecPauliProdResp& resp, const GetExpecPauliProdReq& req)
{
    InitQubitsReq initinfo;
    initinfo.__set_exec_type(ExecCmdType::ExecTypeDefault);
    CStatisticsTimeAndCount countsis(initinfo.exec_type, Tag_Iinterface_getExpecPauliProd);

    SINGLETON(CTaskManager)->getExpecPauliProd(resp, req, initinfo);
    countsis.setExecType(initinfo.exec_type);
    countsis.setQubitNum(initinfo.qubits);

    CStatisticsCodeCount codesis(initinfo.exec_type, Tag_Iinterface_getExpecPauliProd, initinfo.qubits, resp.base.code);
}

//获取泡利算子乘积之和的期望值
void CSlaveServerHandler::getExpecPauliSum(GetExpecPauliSumResp& resp, const GetExpecPauliSumReq& req)
{
    InitQubitsReq initinfo;
    initinfo.__set_exec_type(ExecCmdType::ExecTypeDefault);
    CStatisticsTimeAndCount countsis(initinfo.exec_type, Tag_Iinterface_getExpecPauliSum);

    SINGLETON(CTaskManager)->getExpecPauliSum(resp, req, initinfo);
    countsis.setExecType(initinfo.exec_type);
    countsis.setQubitNum(initinfo.qubits);

    CStatisticsCodeCount codesis(initinfo.exec_type, Tag_Iinterface_getExpecPauliSum, initinfo.qubits, resp.base.code);
}

//获取测量结果
void CSlaveServerHandler::measureQubits(MeasureQubitsResp& resp, const MeasureQubitsReq& req)
{
    InitQubitsReq initinfo;
    initinfo.__set_exec_type(ExecCmdType::ExecTypeDefault);
    CStatisticsTimeAndCount countsis(initinfo.exec_type, Tag_Iinterface_measureQubits);

    SINGLETON(CTaskManager)->measureQubits(resp, req, initinfo);
    countsis.setExecType(initinfo.exec_type);
    countsis.setQubitNum(initinfo.qubits);

    CStatisticsCodeCount codesis(initinfo.exec_type, Tag_Iinterface_measureQubits, initinfo.qubits, resp.base.code);
}

//注册一些自定义量子门，单次任务有效
void CSlaveServerHandler::addCustomGateByMatrix(AddCustomGateByMatrixResp& resp, const AddCustomGateByMatrixReq& req)
{
    InitQubitsReq initinfo;
    initinfo.__set_exec_type(ExecCmdType::ExecTypeDefault);
    CStatisticsTimeAndCount countsis(initinfo.exec_type, Tag_Iinterface_addCustomGateByMatrix);

    SINGLETON(CTaskManager)->addCustomGateByMatrix(resp, req, initinfo);
    countsis.setExecType(initinfo.exec_type);
    countsis.setQubitNum(initinfo.qubits);

    CStatisticsCodeCount codesis(initinfo.exec_type, Tag_Iinterface_addCustomGateByMatrix, initinfo.qubits, resp.base.code);
}

//添加量子门操作
void CSlaveServerHandler::addSubCircuit(AddSubCircuitResp& resp, const AddSubCircuitReq& req)
{
    InitQubitsReq initinfo;
    initinfo.__set_exec_type(ExecCmdType::ExecTypeDefault);
    CStatisticsTimeAndCount countsis(initinfo.exec_type, Tag_Iinterface_addSubCircuit);

    SINGLETON(CTaskManager)->addSubCircuit(resp, req, initinfo);
    countsis.setExecType(initinfo.exec_type);
    countsis.setQubitNum(initinfo.qubits);

    CStatisticsCodeCount codesis(initinfo.exec_type, Tag_Iinterface_addSubCircuit, initinfo.qubits, resp.base.code);
}

//追加量子比特到当前的量子电路
void CSlaveServerHandler::appendQubits(AppendQubitsResp& resp, const AppendQubitsReq& req)
{
    InitQubitsReq initinfo;
    initinfo.__set_exec_type(ExecCmdType::ExecTypeDefault);
    CStatisticsTimeAndCount countsis(initinfo.exec_type, Tag_Iinterface_appendQubits);

    SINGLETON(CTaskManager)->appendQubits(resp, req, initinfo);
    countsis.setExecType(initinfo.exec_type);
    countsis.setQubitNum(initinfo.qubits);

    CStatisticsCodeCount codesis(initinfo.exec_type, Tag_Iinterface_appendQubits, initinfo.qubits, resp.base.code);
}

//重置指定的qubits
void CSlaveServerHandler::resetQubits(ResetQubitsResp& resp, const ResetQubitsReq& req)
{
    InitQubitsReq initinfo;
    initinfo.__set_exec_type(ExecCmdType::ExecTypeDefault);
    CStatisticsTimeAndCount countsis(initinfo.exec_type, Tag_Iinterface_resetQubits);

    SINGLETON(CTaskManager)->resetQubits(resp, req, initinfo);
    countsis.setExecType(initinfo.exec_type);
    countsis.setQubitNum(initinfo.qubits);

    CStatisticsCodeCount codesis(initinfo.exec_type, Tag_Iinterface_resetQubits, initinfo.qubits, resp.base.code);
}

//获取当前量子状态向量
void CSlaveServerHandler::getStateOfAllQubits(GetStateOfAllQubitsResp& resp, const GetStateOfAllQubitsReq& req)
{
    InitQubitsReq initinfo;
    initinfo.__set_exec_type(ExecCmdType::ExecTypeDefault);
    CStatisticsTimeAndCount countsis(initinfo.exec_type, Tag_Iinterface_getStateOfAllQubits);

    SINGLETON(CTaskManager)->getStateOfAllQubits(resp, req, initinfo);
    countsis.setExecType(initinfo.exec_type);
    countsis.setQubitNum(initinfo.qubits);

    CStatisticsCodeCount codesis(initinfo.exec_type, Tag_Iinterface_getStateOfAllQubits, initinfo.qubits, resp.base.code);
}

//获取当前所有可能状态组合的概率
void CSlaveServerHandler::getProbabilities(GetProbabilitiesResp& resp, const GetProbabilitiesReq& req)
{
    InitQubitsReq initinfo;
    initinfo.__set_exec_type(ExecCmdType::ExecTypeDefault);
    CStatisticsTimeAndCount countsis(initinfo.exec_type, Tag_Iinterface_getProbabilities);

    SINGLETON(CTaskManager)->getProbabilities(resp, req, initinfo);
    countsis.setExecType(initinfo.exec_type);
    countsis.setQubitNum(initinfo.qubits);

    CStatisticsCodeCount codesis(initinfo.exec_type, Tag_Iinterface_getProbabilities, initinfo.qubits, resp.base.code);
}
