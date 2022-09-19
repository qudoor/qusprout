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

    try
    {
       SINGLETON(CTaskManager)->initQubits(resp, req);
    }
    catch(const TTransportException& e)
    {
        setBase(resp.base, ErrCode::type::COM_EXCEPTION);
        LOG(ERROR) << "initQubits exception(req:" << reqbuf << ",err:" << e.what() << ").";
    }
    catch(...)
    {
        setBase(resp.base, ErrCode::type::COM_EXCEPTION);
        LOG(ERROR) << "initQubits other exception(req:" << reqbuf << ").";
    }
    
    LOG(INFO) << "response initQubits(id:" << req.id << ",resp:" << getPrint<InitQubitsResp>(resp) << ").";
}

//发送任务
void CQuSproutServerHandler::sendCircuitCmd(SendCircuitCmdResp& resp, const SendCircuitCmdReq& req)
{
    try
    {
        SINGLETON(CTaskManager)->sendCircuitCmd(resp, req);
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
}

//取消任务
void CQuSproutServerHandler::cancelCmd(CancelCmdResp& resp, const CancelCmdReq& req)
{
    std::string reqbuf = getPrint<CancelCmdReq>(req);
    LOG(INFO) << "request CancelCmdReq(req:" << reqbuf << ").";

    try
    {
        SINGLETON(CTaskManager)->cancelCmd(resp, req);
    }
    catch(const TTransportException& e)
    {
        setBase(resp.base, ErrCode::type::COM_EXCEPTION);
        LOG(ERROR) << "cancelCmd exception(req:" << reqbuf << ",err:" << e.what() << ").";
    }
    catch(...)
    {
        setBase(resp.base, ErrCode::type::COM_EXCEPTION);
        LOG(ERROR) << "cancelCmd other exception(req:" << reqbuf << ").";
    }

    LOG(INFO) << "response CancelCmdReq(id:" << req.id << ",resp:" << getPrint<CancelCmdResp>(resp) << ").";
}

//获取振幅
void CQuSproutServerHandler::getProbAmp(GetProbAmpResp& resp, const GetProbAmpReq& req)
{
    try
    {
        SINGLETON(CTaskManager)->getProbAmp(resp, req);
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
}

//获取当前qubit的概率
void CQuSproutServerHandler::getProbOfOutcome(GetProbOfOutcomeResp& resp, const GetProbOfOutcomeReq& req)
{
    try
    {
        SINGLETON(CTaskManager)->getProbOfOutcome(resp, req);
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
}

//获取所有qubit的概率
void CQuSproutServerHandler::getProbOfAllOutcome(GetProbOfAllOutcomResp& resp, const GetProbOfAllOutcomReq& req)
{
    try
    {
        SINGLETON(CTaskManager)->getProbOfAllOutcome(resp, req);
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
}

//获取所有的计算结果
void CQuSproutServerHandler::getAllState(GetAllStateResp& resp, const GetAllStateReq& req)
{
    try
    {
        SINGLETON(CTaskManager)->getAllState(resp, req);
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
}

//执行任务
void CQuSproutServerHandler::run(RunCircuitResp& resp, const RunCircuitReq& req)
{
    try
    {
        SINGLETON(CTaskManager)->run(resp, req);
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
}

//对部分量子比特应用量子傅立叶变换
void CQuSproutServerHandler::applyQFT(ApplyQFTResp& resp, const ApplyQFTReq& req)
{
    try
    {
        SINGLETON(CTaskManager)->applyQFT(resp, req);
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
}

//对所有量子比特应用量子傅立叶变换
void CQuSproutServerHandler::applyFullQFT(ApplyFullQFTResp& resp, const ApplyFullQFTReq& req)
{
    try
    {
        SINGLETON(CTaskManager)->applyFullQFT(resp, req);
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
}

//获取泡利算子乘积的期望值
void CQuSproutServerHandler::getExpecPauliProd(GetExpecPauliProdResp& resp, const GetExpecPauliProdReq& req)
{
    try
    {
        SINGLETON(CTaskManager)->getExpecPauliProd(resp, req);
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
}

//获取泡利算子乘积之和的期望值
void CQuSproutServerHandler::getExpecPauliSum(GetExpecPauliSumResp& resp, const GetExpecPauliSumReq& req)
{
    try
    {
        SINGLETON(CTaskManager)->getExpecPauliSum(resp, req);
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
}
