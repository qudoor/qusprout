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

    try
    {
       SINGLETON(CTaskManager)->initQubits(resp, req);
    }
    catch(const TTransportException& e)
    {
        setBase(resp.base, ErrCode::type::COM_EXCEPTION);
        std::string reqbuf = getPrint<InitQubitsReq>(req);
        LOG(ERROR) << "initQubits exception(req:" << reqbuf << ",err:" << e.what() << ").";
    }
    catch(...)
    {
        setBase(resp.base, ErrCode::type::COM_EXCEPTION);
        std::string reqbuf = getPrint<InitQubitsReq>(req);
        LOG(ERROR) << "initQubits other exception(req:" << reqbuf << ").";
    }

    CStatisticsCodeCount codesis(req.exec_type, Tag_Iinterface_initQubits, req.qubits, resp.base.code);
}

//发送任务
void CSlaveServerHandler::sendCircuitCmd(SendCircuitCmdResp& resp, const SendCircuitCmdReq& req)
{
    InitQubitsReq initinfo;
    initinfo.__set_exec_type(ExecCmdType::ExecTypeDefault);
    CStatisticsTimeAndCount countsis(initinfo.exec_type, Tag_Iinterface_sendCircuitCmd);

    try
    {
        SINGLETON(CTaskManager)->sendCircuitCmd(resp, req, initinfo);
        countsis.setExecType(initinfo.exec_type);
        countsis.setQubitNum(initinfo.qubits);
    }
    catch(const TTransportException& e)
    {
        setBase(resp.base, ErrCode::type::COM_EXCEPTION);
        std::string reqbuf = getPrint<SendCircuitCmdReq>(req);
        LOG(ERROR) << "sendCircuitCmd exception(req:" << reqbuf << ",err:" << e.what() << ").";
    }
    catch(...)
    {
        setBase(resp.base, ErrCode::type::COM_EXCEPTION);
        std::string reqbuf = getPrint<SendCircuitCmdReq>(req);
        LOG(ERROR) << "sendCircuitCmd other exception(req:" << reqbuf << ").";
    }

    CStatisticsCodeCount codesis(initinfo.exec_type, Tag_Iinterface_sendCircuitCmd, initinfo.qubits, resp.base.code);
}

//取消任务
void CSlaveServerHandler::cancelCmd(CancelCmdResp& resp, const CancelCmdReq& req)
{
    InitQubitsReq initinfo;
    initinfo.__set_exec_type(ExecCmdType::ExecTypeDefault);
    CStatisticsTimeAndCount countsis(initinfo.exec_type, Tag_Iinterface_cancelCmd);

    try
    {
        SINGLETON(CTaskManager)->cancelCmd(resp, req, initinfo);
        countsis.setExecType(initinfo.exec_type);
        countsis.setQubitNum(initinfo.qubits);
    }
    catch(const TTransportException& e)
    {
        setBase(resp.base, ErrCode::type::COM_EXCEPTION);
        std::string reqbuf = getPrint<CancelCmdReq>(req);
        LOG(ERROR) << "cancelCmd exception(req:" << reqbuf << ",err:" << e.what() << ").";
    }
    catch(...)
    {
        setBase(resp.base, ErrCode::type::COM_EXCEPTION);
        std::string reqbuf = getPrint<CancelCmdReq>(req);
        LOG(ERROR) << "cancelCmd other exception(req:" << reqbuf << ").";
    }

    CStatisticsCodeCount codesis(initinfo.exec_type, Tag_Iinterface_cancelCmd, initinfo.qubits, resp.base.code);
}

//获取振幅
void CSlaveServerHandler::getProbAmp(GetProbAmpResp& resp, const GetProbAmpReq& req)
{
    InitQubitsReq initinfo;
    initinfo.__set_exec_type(ExecCmdType::ExecTypeDefault);
    CStatisticsTimeAndCount countsis(initinfo.exec_type, Tag_Iinterface_getProbAmp);

    try
    {
        SINGLETON(CTaskManager)->getProbAmp(resp, req, initinfo);
        countsis.setExecType(initinfo.exec_type);
        countsis.setQubitNum(initinfo.qubits);
    }
    catch(const TTransportException& e)
    {
        setBase(resp.base, ErrCode::type::COM_EXCEPTION);
        std::string reqbuf = getPrint<GetProbAmpReq>(req);
        LOG(ERROR) << "getProbAmp exception(req:" << reqbuf << ",err:" << e.what() << ").";
    }
    catch(...)
    {
        setBase(resp.base, ErrCode::type::COM_EXCEPTION);
        std::string reqbuf = getPrint<GetProbAmpReq>(req);
        LOG(ERROR) << "getProbAmp other exception(req:" << reqbuf << ").";
    }

    CStatisticsCodeCount codesis(initinfo.exec_type, Tag_Iinterface_getProbAmp, initinfo.qubits, resp.base.code);
}

//获取当前qubit的概率
void CSlaveServerHandler::getProbOfOutcome(GetProbOfOutcomeResp& resp, const GetProbOfOutcomeReq& req)
{
    InitQubitsReq initinfo;
    initinfo.__set_exec_type(ExecCmdType::ExecTypeDefault);
    CStatisticsTimeAndCount countsis(initinfo.exec_type, Tag_Iinterface_getProbOfOutcome);

    try
    {
        SINGLETON(CTaskManager)->getProbOfOutcome(resp, req, initinfo);
        countsis.setExecType(initinfo.exec_type);
        countsis.setQubitNum(initinfo.qubits);
    }
    catch(const TTransportException& e)
    {
        setBase(resp.base, ErrCode::type::COM_EXCEPTION);
        std::string reqbuf = getPrint<GetProbOfOutcomeReq>(req);
        LOG(ERROR) << "getProbOfOutcome exception(req:" << reqbuf << ",err:" << e.what() << ").";
    }
    catch(...)
    {
        setBase(resp.base, ErrCode::type::COM_EXCEPTION);
        std::string reqbuf = getPrint<GetProbOfOutcomeReq>(req);
        LOG(ERROR) << "getProbOfOutcome other exception(req:" << reqbuf << ").";
    }

    CStatisticsCodeCount codesis(initinfo.exec_type, Tag_Iinterface_getProbOfOutcome, initinfo.qubits, resp.base.code);
}

//获取所有qubit的概率
void CSlaveServerHandler::getProbOfAllOutcome(GetProbOfAllOutcomResp& resp, const GetProbOfAllOutcomReq& req)
{
    InitQubitsReq initinfo;
    initinfo.__set_exec_type(ExecCmdType::ExecTypeDefault);
    CStatisticsTimeAndCount countsis(initinfo.exec_type, Tag_Iinterface_getProbOfAllOutcome);

    try
    {
        SINGLETON(CTaskManager)->getProbOfAllOutcome(resp, req, initinfo);
        countsis.setExecType(initinfo.exec_type);
        countsis.setQubitNum(initinfo.qubits);
    }
    catch(const TTransportException& e)
    {
        setBase(resp.base, ErrCode::type::COM_EXCEPTION);
        std::string reqbuf = getPrint<GetProbOfAllOutcomReq>(req);
        LOG(ERROR) << "getProbOfAllOutcome exception(req:" << reqbuf << ",err:" << e.what() << ").";
    }
    catch(...)
    {
        setBase(resp.base, ErrCode::type::COM_EXCEPTION);
        std::string reqbuf = getPrint<GetProbOfAllOutcomReq>(req);
        LOG(ERROR) << "getProbOfAllOutcome other exception(req:" << reqbuf << ").";
    }

    CStatisticsCodeCount codesis(initinfo.exec_type, Tag_Iinterface_getProbOfAllOutcome, initinfo.qubits, resp.base.code);
}

//获取所有的计算结果
void CSlaveServerHandler::getAllState(GetAllStateResp& resp, const GetAllStateReq& req)
{
    InitQubitsReq initinfo;
    initinfo.__set_exec_type(ExecCmdType::ExecTypeDefault);
    CStatisticsTimeAndCount countsis(initinfo.exec_type, Tag_Iinterface_getAllState);

    try
    {
        SINGLETON(CTaskManager)->getAllState(resp, req, initinfo);
        countsis.setExecType(initinfo.exec_type);
        countsis.setQubitNum(initinfo.qubits);
    }
    catch(const TTransportException& e)
    {
        setBase(resp.base, ErrCode::type::COM_EXCEPTION);
        std::string reqbuf = getPrint<GetAllStateReq>(req);
        LOG(ERROR) << "getAllState exception(req:" << reqbuf << ",err:" << e.what() << ").";
    }
    catch(...)
    {
        setBase(resp.base, ErrCode::type::COM_EXCEPTION);
        std::string reqbuf = getPrint<GetAllStateReq>(req);
        LOG(ERROR) << "getAllState other exception(req:" << reqbuf << ").";
    }

    CStatisticsCodeCount codesis(initinfo.exec_type, Tag_Iinterface_getAllState, initinfo.qubits, resp.base.code);
}

//执行任务
void CSlaveServerHandler::run(RunCircuitResp& resp, const RunCircuitReq& req)
{
    InitQubitsReq initinfo;
    initinfo.__set_exec_type(ExecCmdType::ExecTypeDefault);
    CStatisticsTimeAndCount countsis(initinfo.exec_type, Tag_Iinterface_run);

    try
    {
        SINGLETON(CTaskManager)->run(resp, req, initinfo);
        countsis.setExecType(initinfo.exec_type);
        countsis.setQubitNum(initinfo.qubits);
    }
    catch(const TTransportException& e)
    {
        setBase(resp.base, ErrCode::type::COM_EXCEPTION);
        std::string reqbuf = getPrint<RunCircuitReq>(req);
        LOG(ERROR) << "run exception(req:" << reqbuf << ",err:" << e.what() << ").";
    }
    catch(...)
    {
        setBase(resp.base, ErrCode::type::COM_EXCEPTION);
        std::string reqbuf = getPrint<RunCircuitReq>(req);
        LOG(ERROR) << "run other exception(req:" << reqbuf << ").";
    }

    CStatisticsCodeCount codesis(initinfo.exec_type, Tag_Iinterface_run, initinfo.qubits, resp.base.code);
}

//对部分量子比特应用量子傅立叶变换
void CSlaveServerHandler::applyQFT(ApplyQFTResp& resp, const ApplyQFTReq& req)
{
    InitQubitsReq initinfo;
    initinfo.__set_exec_type(ExecCmdType::ExecTypeDefault);
    CStatisticsTimeAndCount countsis(initinfo.exec_type, Tag_Iinterface_applyQFT);

    try
    {
        SINGLETON(CTaskManager)->applyQFT(resp, req, initinfo);
        countsis.setExecType(initinfo.exec_type);
        countsis.setQubitNum(initinfo.qubits);
    }
    catch(const TTransportException& e)
    {
        setBase(resp.base, ErrCode::type::COM_EXCEPTION);
        std::string reqbuf = getPrint<ApplyQFTReq>(req);
        LOG(ERROR) << "applyQFT exception(req:" << reqbuf << ",err:" << e.what() << ").";
    }
    catch(...)
    {
        setBase(resp.base, ErrCode::type::COM_EXCEPTION);
        std::string reqbuf = getPrint<ApplyQFTReq>(req);
        LOG(ERROR) << "applyQFT other exception(req:" << reqbuf << ").";
    }

    CStatisticsCodeCount codesis(initinfo.exec_type, Tag_Iinterface_applyQFT, initinfo.qubits, resp.base.code);
}

//对所有量子比特应用量子傅立叶变换
void CSlaveServerHandler::applyFullQFT(ApplyFullQFTResp& resp, const ApplyFullQFTReq& req)
{
    InitQubitsReq initinfo;
    initinfo.__set_exec_type(ExecCmdType::ExecTypeDefault);
    CStatisticsTimeAndCount countsis(initinfo.exec_type, Tag_Iinterface_applyFullQFT);

    try
    {
        SINGLETON(CTaskManager)->applyFullQFT(resp, req, initinfo);
        countsis.setExecType(initinfo.exec_type);
        countsis.setQubitNum(initinfo.qubits);
    }
    catch(const TTransportException& e)
    {
        setBase(resp.base, ErrCode::type::COM_EXCEPTION);
        std::string reqbuf = getPrint<ApplyFullQFTReq>(req);
        LOG(ERROR) << "applyFullQFT exception(req:" << reqbuf << ",err:" << e.what() << ").";
    }
    catch(...)
    {
        setBase(resp.base, ErrCode::type::COM_EXCEPTION);
        std::string reqbuf = getPrint<ApplyFullQFTReq>(req);
        LOG(ERROR) << "applyFullQFT other exception(req:" << reqbuf << ").";
    }

    CStatisticsCodeCount codesis(initinfo.exec_type, Tag_Iinterface_applyFullQFT, initinfo.qubits, resp.base.code);
}

//获取泡利算子乘积的期望值
void CSlaveServerHandler::getExpecPauliProd(GetExpecPauliProdResp& resp, const GetExpecPauliProdReq& req)
{
    InitQubitsReq initinfo;
    initinfo.__set_exec_type(ExecCmdType::ExecTypeDefault);
    CStatisticsTimeAndCount countsis(initinfo.exec_type, Tag_Iinterface_getExpecPauliProd);

    try
    {
        SINGLETON(CTaskManager)->getExpecPauliProd(resp, req, initinfo);
        countsis.setExecType(initinfo.exec_type);
        countsis.setQubitNum(initinfo.qubits);
    }
    catch(const TTransportException& e)
    {
        setBase(resp.base, ErrCode::type::COM_EXCEPTION);
        std::string reqbuf = getPrint<GetExpecPauliProdReq>(req);
        LOG(ERROR) << "getExpecPauliProd exception(req:" << reqbuf << ",err:" << e.what() << ").";
    }
    catch(...)
    {
        setBase(resp.base, ErrCode::type::COM_EXCEPTION);
        std::string reqbuf = getPrint<GetExpecPauliProdReq>(req);
        LOG(ERROR) << "getExpecPauliProd other exception(req:" << reqbuf << ").";
    }

    CStatisticsCodeCount codesis(initinfo.exec_type, Tag_Iinterface_getExpecPauliProd, initinfo.qubits, resp.base.code);
}

//获取泡利算子乘积之和的期望值
void CSlaveServerHandler::getExpecPauliSum(GetExpecPauliSumResp& resp, const GetExpecPauliSumReq& req)
{
    InitQubitsReq initinfo;
    initinfo.__set_exec_type(ExecCmdType::ExecTypeDefault);
    CStatisticsTimeAndCount countsis(initinfo.exec_type, Tag_Iinterface_getExpecPauliSum);

    try
    {
        SINGLETON(CTaskManager)->getExpecPauliSum(resp, req, initinfo);
        countsis.setExecType(initinfo.exec_type);
        countsis.setQubitNum(initinfo.qubits);
    }
    catch(const TTransportException& e)
    {
        setBase(resp.base, ErrCode::type::COM_EXCEPTION);
        std::string reqbuf = getPrint<GetExpecPauliSumReq>(req);
        LOG(ERROR) << "getExpecPauliSum exception(req:" << reqbuf << ",err:" << e.what() << ").";
    }
    catch(...)
    {
        setBase(resp.base, ErrCode::type::COM_EXCEPTION);
        std::string reqbuf = getPrint<GetExpecPauliSumReq>(req);
        LOG(ERROR) << "getExpecPauliSum other exception(req:" << reqbuf << ").";
    }

    CStatisticsCodeCount codesis(initinfo.exec_type, Tag_Iinterface_getExpecPauliSum, initinfo.qubits, resp.base.code);
}
