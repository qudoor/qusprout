#include <iostream>
#include <sstream>

#include "../../common/Singleton.h"
#include "../../common/qulog.h"
#include "../interface/ecode_constants.h"
#include "../common/QuESTIniter.h"

#include "WorkServerHandler.h"
#include "../common/CmdParams.h"

CWorkServerHandler::CWorkServerHandler()
{
}

CWorkServerHandler::~CWorkServerHandler()
{
}

//qubit初始化
void CWorkServerHandler::initQubits(InitQubitsResp& resp, const InitQubitsReq& req)
{
    // try
    // {
    //     if (req.id.empty() || req.qubits <= 0)
    //     {
    //         LOG(ERROR) << "Invaild parameters(req.id.empty() || req.qubits <= 0) in CWorkServerHandler::initQubits()";
    //         setBase(resp.base, ErrCode::type::COM_INVALID_PARAM);
    //         return;
    //     }

    //     m_executor.init(req.qubits, req.state, req.density);
    //     setBase(resp.base, ErrCode::type::COM_SUCCESS);
    // }
    // catch(const std::exception& e)
    // {
    //     LOG(ERROR) << "CWorkServerHandler::initQubits exception: " << e.what();
    //     setBase(resp.base, ErrCode::type::COM_OTHRE);
    // }
}

//执行任务
void CWorkServerHandler::sendCircuitCmd(SendCircuitCmdResp& resp, const SendCircuitCmdReq& req)
{
    try
    {
        if (req.id.empty() || (req.final == false && req.circuit.cmds.size() <= 0))
        {
            LOG(ERROR) << "Invaild parameters(req.id.empty() || (req.final == false && req.circuit.cmds.size() <= 0)) in CWorkServerHandler::sendCircuitCmd()";
            setBase(resp.base, ErrCode::type::COM_INVALID_PARAM);
            return;
        }

        //Broadcast circuit to all node
        if (SINGLETON(CmdParams)->execType == (int)ExecCmdType::type::ExecTypeCpuMpi)
            workNodeHandler.sendCircuitCmd(req.circuit.cmds, req.final);

        m_executor.receiveCircuit(req.circuit, req.final);
        setBase(resp.base, ErrCode::type::COM_SUCCESS);
    }
    catch(const std::exception& e)
    {
        LOG(ERROR) << "CWorkServerHandler::sendCircuitCmd exception: " << e.what();
        setBase(resp.base, ErrCode::type::COM_OTHRE);
    }
}

//取消任务
void CWorkServerHandler::cancelCmd(CancelCmdResp& resp, const CancelCmdReq& req)
{
    try
    {
        if (req.id.empty())
        {
            LOG(ERROR) << "Invaild parameters(req.id.empty()) in CWorkServerHandler::cancelCmd()";
            setBase(resp.base, ErrCode::type::COM_INVALID_PARAM);
            return;
        }

        //Broadcast cancel to all node
        if (SINGLETON(CmdParams)->execType == (int)ExecCmdType::type::ExecTypeCpuMpi)
            workNodeHandler.cancelCmd();

        if (m_server)
            m_server->stop();

        setBase(resp.base, ErrCode::type::COM_SUCCESS);
    }
    catch(const std::exception& e)
    {
        LOG(ERROR) << "CWorkServerHandler::cancelCmd exception: " << e.what();
        setBase(resp.base, ErrCode::type::COM_OTHRE);
    }
}

//获取振幅
void CWorkServerHandler::getProbAmp(GetProbAmpResp& resp, const GetProbAmpReq& req)
{
    try
    {
        if (req.id.empty())
        {
            LOG(ERROR) << "Invaild parameters(req.id.empty()) in CWorkServerHandler::getProbAmp()";
            setBase(resp.base, ErrCode::type::COM_INVALID_PARAM);
            return;
        }

        //Broadcast getProbAmp to all node
        if (SINGLETON(CmdParams)->execType == (int)ExecCmdType::type::ExecTypeCpuMpi)
            workNodeHandler.getProbAmp(req.index);

        resp.__set_amp(m_executor.getProbOfAmp(req.index));
        setBase(resp.base, ErrCode::type::COM_SUCCESS);
    }
    catch(const std::exception& e)
    {
        LOG(ERROR) << "CWorkServerHandler::getProbAmp exception: " << e.what();
        setBase(resp.base, ErrCode::type::COM_OTHRE);
    }
}

//获取当前qubit的概率
void CWorkServerHandler::getProbOfOutcome(GetProbOfOutcomeResp& resp, const GetProbOfOutcomeReq& req)
{
    try
    {
        if (req.id.empty())
        {
            LOG(ERROR) << "Invaild parameters(req.id.empty()) in CWorkServerHandler::getProbOfOutcome()";
            setBase(resp.base, ErrCode::type::COM_INVALID_PARAM);
            return;
        }

        //Broadcast getProbOutcome to all node
        if (SINGLETON(CmdParams)->execType == (int)ExecCmdType::type::ExecTypeCpuMpi)
            workNodeHandler.getProbOutcome(req.qubit, req.outcom);
        
        resp.__set_pro_outcome(m_executor.getProbOfOutcome(req.qubit, req.outcom));
        setBase(resp.base, ErrCode::type::COM_SUCCESS);
    }
    catch(const std::exception& e)
    {
        LOG(ERROR) << "CWorkServerHandler::getProbOfOutcome exception: " << e.what();
        setBase(resp.base, ErrCode::type::COM_OTHRE);
    }
}

//获取所有qubit的概率
void CWorkServerHandler::getProbOfAllOutcome(GetProbOfAllOutcomResp& resp, const GetProbOfAllOutcomReq& req)
{
    try
    {
        if (req.id.empty())
        {
            LOG(ERROR) << "Invaild parameters(req.id.empty()) in CWorkServerHandler::getProbOfAllOutcome()";
            setBase(resp.base, ErrCode::type::COM_INVALID_PARAM);
            return;
        }

        //Broadcast getProbOfAllOutcome to all node
        if (SINGLETON(CmdParams)->execType == (int)ExecCmdType::type::ExecTypeCpuMpi)
            workNodeHandler.getProbOfAllOutcome(req.targets);

        std::vector<double> pro_outcomes;
        m_executor.getProbOfAllOutcome(pro_outcomes, req.targets);
        resp.__set_pro_outcomes(pro_outcomes);
        setBase(resp.base, ErrCode::type::COM_SUCCESS);
    }
    catch(const std::exception& e)
    {
        LOG(ERROR) << "CWorkServerHandler::getProbOfAllOutcome exception: " << e.what();
        setBase(resp.base, ErrCode::type::COM_OTHRE);
    }
}

//获取所有的计算结果
void CWorkServerHandler::getAllState(GetAllStateResp& resp, const GetAllStateReq& req)
{
    try
    {
        if (req.id.empty())
        {
            LOG(ERROR) << "Invaild parameters(req.id.empty()) in CWorkServerHandler::getAllState()";
            setBase(resp.base, ErrCode::type::COM_INVALID_PARAM);
            return;
        }

        //Broadcast getAllState to all node
        if (SINGLETON(CmdParams)->execType == (int)ExecCmdType::type::ExecTypeCpuMpi)
            workNodeHandler.getAllState();

        std::vector<std::string> all_state;
        m_executor.getAllState(all_state);
        resp.__set_all_state(all_state);
        setBase(resp.base, ErrCode::type::COM_SUCCESS);
    }
    catch(const std::exception& e)
    {
        LOG(ERROR) << "CWorkServerHandler::getAllState exception: " << e.what();
        setBase(resp.base, ErrCode::type::COM_OTHRE);
    }
}

//执行任务
void CWorkServerHandler::run(RunCircuitResp& resp, const RunCircuitReq& req)
{
    try
    {
        if (req.id.empty())
        {
            LOG(ERROR) << "Invaild parameters(req.id.empty()) in CWorkServerHandler::run()";
            setBase(resp.base, ErrCode::type::COM_INVALID_PARAM);
            return;
        }

        //Broadcast run to all node
        if (SINGLETON(CmdParams)->execType == (int)ExecCmdType::type::ExecTypeCpuMpi)
            workNodeHandler.run(req.shots);

        Result result;
        m_executor.run(result, req.shots);
        resp.__set_result(result);
        setBase(resp.base, ErrCode::type::COM_SUCCESS);
    }
    catch(const std::exception& e)
    {
        LOG(ERROR) << "CWorkServerHandler::applyQFT exception: " << e.what();
        setBase(resp.base, ErrCode::type::COM_OTHRE);
    }
}

//对部分量子比特应用量子傅立叶变换
void CWorkServerHandler::applyQFT(ApplyQFTResp& resp, const ApplyQFTReq& req)
{
    try
    {
        if (req.id.empty())
        {
            LOG(ERROR) << "Invaild parameters(req.id.empty()) in CWorkServerHandler::applyQFT()";
            setBase(resp.base, ErrCode::type::COM_INVALID_PARAM);
            return;
        }

        //Broadcast apply_QFT to all node
        if (SINGLETON(CmdParams)->execType == (int)ExecCmdType::type::ExecTypeCpuMpi)
            workNodeHandler.applyQFT(req.targets);

        m_executor.apply_QFT(req.targets);
        setBase(resp.base, ErrCode::type::COM_SUCCESS);
    }
    catch(const std::exception& e)
    {
        LOG(ERROR) << "CWorkServerHandler::applyQFT exception: " << e.what();
        setBase(resp.base, ErrCode::type::COM_OTHRE);
    }
}

//对所有量子比特应用量子傅立叶变换
void CWorkServerHandler::applyFullQFT(ApplyFullQFTResp& resp, const ApplyFullQFTReq& req)
{
    try
    {
        if (req.id.empty())
        {
            LOG(ERROR) << "Invaild parameters(req.id.empty()) in CWorkServerHandler::applyFullQFT()";
            setBase(resp.base, ErrCode::type::COM_INVALID_PARAM);
            return;
        }

        //Broadcast apply_Full_QFT to all node
        if (SINGLETON(CmdParams)->execType == (int)ExecCmdType::type::ExecTypeCpuMpi)
            workNodeHandler.applyFullQFT();

        m_executor.apply_Full_QFT();
        setBase(resp.base, ErrCode::type::COM_SUCCESS);
    }
    catch(const std::exception& e)
    {
        LOG(ERROR) << "CWorkServerHandler::applyFullQFT exception: " << e.what();
        setBase(resp.base, ErrCode::type::COM_OTHRE);
    }
}

//获取泡利算子乘积的期望值
void CWorkServerHandler::getExpecPauliProd(GetExpecPauliProdResp& resp, const GetExpecPauliProdReq& req)
{
    try
    {
        if (req.id.empty())
        {
            LOG(ERROR) << "Invaild parameters(req.id.empty()) in CWorkServerHandler::getExpecPauliProd()";
            setBase(resp.base, ErrCode::type::COM_INVALID_PARAM);
            return;
        }

        if (SINGLETON(CmdParams)->execType == (int)ExecCmdType::type::ExecTypeCpuMpi)
            workNodeHandler.getExpecPauliProd(req.pauli_prod);

        double expect = m_executor.getExpecPauliProd(req.pauli_prod);
        resp.__set_expect(expect);
        setBase(resp.base, ErrCode::type::COM_SUCCESS);
    }
    catch(const std::exception& e)
    {
        LOG(ERROR) << "CWorkServerHandler::getExpecPauliProd exception: " << e.what();
        setBase(resp.base, ErrCode::type::COM_OTHRE);
    }
}

//获取泡利算子乘积之和的期望值
void CWorkServerHandler::getExpecPauliSum(GetExpecPauliSumResp& resp, const GetExpecPauliSumReq& req)
{
    try
    {
        if (req.id.empty())
        {
            LOG(ERROR) << "Invaild parameters(req.id.empty()) in CWorkServerHandler::getExpecPauliSum()";
            setBase(resp.base, ErrCode::type::COM_INVALID_PARAM);
            return;
        }

        if (SINGLETON(CmdParams)->execType == (int)ExecCmdType::type::ExecTypeCpuMpi)
            workNodeHandler.getExpecPauliSum(req);

        double expect = m_executor.getExpecPauliSum(req.oper_type_list, req.term_coeff_list);
        resp.__set_expect(expect);
        setBase(resp.base, ErrCode::type::COM_SUCCESS);
    }
    catch(const std::exception& e)
    {
        LOG(ERROR) << "CWorkServerHandler::getExpecPauliSum exception: " << e.what();
        setBase(resp.base, ErrCode::type::COM_OTHRE);
    }
}

//设置server 
void CWorkServerHandler::SetThreadedServer(std::shared_ptr<TThreadedServer> server)
{
    m_server = server;
}

void CWorkServerHandler::setBase(BaseCode& base, const ErrCode::type& code)
{
    base.__set_code(code);
    auto ptr = g_ecode_constants.ErrMsg.find(code);
    base.__set_msg(ptr->second);
}