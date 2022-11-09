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

#include <iostream>
#include <sstream>
#include "common/Singleton.h"
#include "common/qulog.h"
#include "interface/ecode_constants.h"
#include "common/QuESTIniter.h"
#include "WorkServerHandler.h"
#include "common/CmdParams.h"

CWorkServerHandler::CWorkServerHandler()
{
}

CWorkServerHandler::~CWorkServerHandler()
{
}

//qubit初始化
void CWorkServerHandler::initQubits(InitQubitsResp& resp, const InitQubitsReq& req)
{
    setBase(resp.base, ErrCode::type::COM_SUCCESS);
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
        LOG(ERROR) << "CWorkServerHandler::run exception: " << e.what();
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

//获取测量结果
void CWorkServerHandler::measureQubits(MeasureQubitsResp& resp, const MeasureQubitsReq& req)
{
    try
    {
        if (req.id.empty())
        {
            LOG(ERROR) << "Invaild parameters(req.id.empty()) in CWorkServerHandler::measureQubits()";
            setBase(resp.base, ErrCode::type::COM_INVALID_PARAM);
            return;
        }

        Result result;
        m_executor.getMeasureResult(req.qubits, result);
        resp.__set_results(result.measureSet);
        resp.__set_outcomes(result.outcomeSet);
        setBase(resp.base, ErrCode::type::COM_SUCCESS);
    }
    catch(const std::exception& e)
    {
        LOG(ERROR) << "CWorkServerHandler::measureQubits exception: " << e.what();
        setBase(resp.base, ErrCode::type::COM_OTHRE);
    }
}

//注册一些自定义量子门，单次任务有效
void CWorkServerHandler::addCustomGateByMatrix(AddCustomGateByMatrixResp& resp, const AddCustomGateByMatrixReq& req)
{
    try
    {
        if (req.id.empty())
        {
            LOG(ERROR) << "Invaild parameters(req.id.empty()) in CWorkServerHandler::addCustomGateByMatrix()";
            setBase(resp.base, ErrCode::type::COM_INVALID_PARAM);
            return;
        }

        //Broadcast addCustomGateByMatrix to all node
        if (SINGLETON(CmdParams)->execType == (int)ExecCmdType::type::ExecTypeCpuMpi)
            workNodeHandler.addCustomGateByMatrix(req.gate);

        m_executor.addCustomGateByMatrix(req.gate);
        setBase(resp.base, ErrCode::type::COM_SUCCESS);
    }
    catch(const std::exception& e)
    {
        LOG(ERROR) << "CWorkServerHandler::addCustomGateByMatrix exception: " << e.what();
        setBase(resp.base, ErrCode::type::COM_OTHRE);
    }
}

//添加量子门操作
void CWorkServerHandler::addSubCircuit(AddSubCircuitResp& resp, const AddSubCircuitReq& req)
{
    SendCircuitCmdResp cmdresp;
    SendCircuitCmdReq cmdreq;
    cmdreq.__set_id(req.id);
    cmdreq.__set_circuit(req.sub_circuit.circuit);
    for (auto& temp : cmdreq.circuit.cmds)
    {
        temp.gate = req.sub_circuit.name;
    }
    cmdreq.__set_final(false);
    sendCircuitCmd(cmdresp, cmdreq);
    resp.__set_base(cmdresp.base);
}

//追加量子比特到当前的量子电路
void CWorkServerHandler::appendQubits(AppendQubitsResp& resp, const AppendQubitsReq& req)
{
    try
    {
        if (req.id.empty())
        {
            LOG(ERROR) << "Invaild parameters(req.id.empty()) in CWorkServerHandler::appendQubits()";
            setBase(resp.base, ErrCode::type::COM_INVALID_PARAM);
            return;
        }

        if (SINGLETON(CmdParams)->execType == (int)ExecCmdType::type::ExecTypeCpuMpi)
        {
            LOG(ERROR) << "operator is not support mpi.";
            setBase(resp.base, ErrCode::type::COM_INVALID_PARAM);
            return;
        }

        int ret = m_executor.appendQubits(req.qubits);
        if (ret != 0)
        {
            LOG(ERROR) << "appendQubits failed(ret:" << ret << ").";
            setBase(resp.base, ErrCode::type::COM_OTHRE);
            return;
        }
        setBase(resp.base, ErrCode::type::COM_SUCCESS);
    }
    catch(const std::exception& e)
    {
        LOG(ERROR) << "CWorkServerHandler::appendQubits exception: " << e.what();
        setBase(resp.base, ErrCode::type::COM_OTHRE);
    }
}

//重置指定的qubits
void CWorkServerHandler::resetQubits(ResetQubitsResp& resp, const ResetQubitsReq& req)
{
    try
    {
        if (req.id.empty())
        {
            LOG(ERROR) << "Invaild parameters(req.id.empty()) in CWorkServerHandler::resetQubits()";
            setBase(resp.base, ErrCode::type::COM_INVALID_PARAM);
            return;
        }

        //Broadcast resetQubits to all node
        if (SINGLETON(CmdParams)->execType == (int)ExecCmdType::type::ExecTypeCpuMpi)
            workNodeHandler.resetQubits(req.qubits);

        m_executor.resetQubits(req.qubits);
        setBase(resp.base, ErrCode::type::COM_SUCCESS);
    }
    catch(const std::exception& e)
    {
        LOG(ERROR) << "CWorkServerHandler::resetQubits exception: " << e.what();
        setBase(resp.base, ErrCode::type::COM_OTHRE);
    }
}

//获取当前量子状态向量
void CWorkServerHandler::getStateOfAllQubits(GetStateOfAllQubitsResp& resp, const GetStateOfAllQubitsReq& req)
{
    try
    {
        if (req.id.empty())
        {
            LOG(ERROR) << "Invaild parameters(req.id.empty()) in CWorkServerHandler::getStateOfAllQubits()";
            setBase(resp.base, ErrCode::type::COM_INVALID_PARAM);
            return;
        }

        //Broadcast resetQubits to all node
        if (SINGLETON(CmdParams)->execType == (int)ExecCmdType::type::ExecTypeCpuMpi)
            workNodeHandler.getStateOfAllQubits();

        std::vector<double> real;
        std::vector<double> imag;
        m_executor.getStateOfAllQubits(real, imag);
        size_t statenum = real.size();
        std::vector<double> statevector;
        for (size_t i = 0; i < statenum; ++i)
        {
            statevector.push_back(real[i]);
            statevector.push_back(imag[i]);
        }
        resp.__set_state_vector(statevector);
        setBase(resp.base, ErrCode::type::COM_SUCCESS);
    }
    catch(const std::exception& e)
    {
        LOG(ERROR) << "CWorkServerHandler::getStateOfAllQubits exception: " << e.what();
        setBase(resp.base, ErrCode::type::COM_OTHRE);
    }
}

//获取当前所有可能状态组合的概率
void CWorkServerHandler::getProbabilities(GetProbabilitiesResp& resp, const GetProbabilitiesReq& req)
{
    try
    {
        if (req.id.empty())
        {
            LOG(ERROR) << "Invaild parameters(req.id.empty()) in CWorkServerHandler::getProbabilities()";
            setBase(resp.base, ErrCode::type::COM_INVALID_PARAM);
            return;
        }

        //Broadcast resetQubits to all node
        if (SINGLETON(CmdParams)->execType == (int)ExecCmdType::type::ExecTypeCpuMpi)
            workNodeHandler.getProbabilities();

        std::vector<double> probabilities;
        m_executor.getProbabilities(probabilities);
        resp.__set_probabilities(probabilities);
        setBase(resp.base, ErrCode::type::COM_SUCCESS);
    }
    catch(const std::exception& e)
    {
        LOG(ERROR) << "CWorkServerHandler::getProbabilities exception: " << e.what();
        setBase(resp.base, ErrCode::type::COM_OTHRE);
    }
}

//获取任务状态
void CWorkServerHandler::getTaskInfo(GetTaskInfoResp& resp, const GetTaskInfoReq& req)
{
    setBase(resp.base, ErrCode::type::COM_INVALID_PARAM);
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