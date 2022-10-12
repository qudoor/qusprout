#include "requestmanager.h"
#include "common/qulog.h"
#include "common/Singleton.h"
#include "comm/errorcode.h"
#include "interface/qusproutdata_types.h"
#include "rpcserver/QuSproutServerHandle.h"

CReqManager::CReqManager()
{

}

CReqManager::~CReqManager()
{

}

void CReqManager::initEnv(const std::shared_ptr<InitEnvReq> req, std::shared_ptr<InitEnvResp> resp)
{
    ExecCmdType::type exectype = ExecCmdType::type::ExecTypeDefault;
    if (req->params.exec_type == 1)
    {
        exectype = ExecCmdType::type::ExecTypeCpuSingle;
    }
    if (req->params.exec_type == 2)
    {
        exectype = ExecCmdType::type::ExecTypeCpuMpi;
    }
    else if (req->params.exec_type == 3)
    {
        exectype = ExecCmdType::type::ExecTypeGpuSingle;
    }

    InitQubitsReq initreq;
    initreq.__set_id(req->taskid);
    initreq.__set_qubits(req->params.qubits);
    initreq.__set_density(req->params.density?true:false);
    initreq.__set_exec_type(exectype);
    initreq.__set_async_type(req->params.async);

    InitQubitsResp initresp;
    SINGLETON(CQuSproutServerHandler)->initQubits(initresp, initreq);
    resp->setcode(req->flowid, switchCode(initresp.base.code));
}

void CReqManager::addCmd(const std::shared_ptr<AddGateCmdReq> req, std::shared_ptr<AddGateCmdResp> resp)
{
    SendCircuitCmdReq sendreq;
    sendreq.__set_id(req->taskid);
    sendreq.__set_final(req->params.final?true:false);

    std::vector<Cmd> cmds;
    for (auto temp : req->params.circuits)
    {
        Cmd cmd;
        cmd.__set_gate(temp.gate);
        cmd.__set_desc(temp.qasmdef);
        cmd.__set_inverse(temp.inverse);

        std::vector<int32_t> targets;
        for (auto target : temp.targets)
        {
            targets.push_back(target);
        }
        cmd.__set_targets(targets);

        std::vector<int32_t> controls;
        for (auto control : temp.controls)
        {
            controls.push_back(control);
        }
        cmd.__set_controls(controls);

        std::vector<double> rotations;
        for (auto rotation : temp.rotations)
        {
            rotations.push_back(rotation);
        }
        cmd.__set_rotation(rotations);

        cmds.push_back(cmd);
    }
    Circuit cir;
    cir.__set_cmds(cmds);
    sendreq.__set_circuit(cir);

    SendCircuitCmdResp sendresp;
    SINGLETON(CQuSproutServerHandler)->sendCircuitCmd(sendresp, sendreq);
    resp->setcode(req->flowid, switchCode(sendresp.base.code));
}

void CReqManager::runCmd(const std::shared_ptr<RunGateCmdReq> req, std::shared_ptr<RunGateCmdResp> resp)
{
    RunCircuitReq runreq;
    runreq.__set_id(req->taskid);
    runreq.__set_shots(req->shots);

    RunCircuitResp runresp;
    SINGLETON(CQuSproutServerHandler)->run(runresp, runreq);
    resp->setcode(req->flowid, switchCode(runresp.base.code));

    for (auto temp : runresp.result.measureSet)
    {
        MeasureInfo measure;
        measure.target = temp.id;
        measure.value = temp.value;
        resp->measures.push_back(measure);
    }

    for (auto temp : runresp.result.outcomeSet)
    {
        OutcomeInfo outcome;
        outcome.bitstr =  temp.bitstr;
        outcome.count =  temp.count;
        resp->outcomes.push_back(outcome);
    }
}

void CReqManager::getAmp(const std::shared_ptr<GetAmpReq> req, std::shared_ptr<GetAmpResp> resp)
{
    resp->setcode(req->flowid, COMMON_SUCCESS);

    for (auto index : req->indexs)
    {
        GetProbAmpReq getreq;
        getreq.__set_id(req->taskid);
        getreq.__set_index(index);

        GetProbAmpResp getresp;
        SINGLETON(CQuSproutServerHandler)->getProbAmp(getresp, getreq);
        resp->setcode(req->flowid, switchCode(getresp.base.code));
        if (getresp.base.code != ErrCode::type::COM_SUCCESS)
        {
            break;
        }
        resp->amps.push_back(getresp.amp);
    }
}

void CReqManager::getProb(const std::shared_ptr<GetProbReq> req, std::shared_ptr<GetProbResp> resp)
{
    resp->setcode(req->flowid, COMMON_SUCCESS);
    size_t targetsize = req->targets.size();
    if (0 == targetsize)
    {
        LOG(ERROR) << "getProb target is empty(taskid:" << req->taskid << ").";
        return;
    }

    if (req->targets.size() == 1)
    {
        GetProbOfOutcomeReq getreq;
        getreq.__set_id(req->taskid);
        getreq.__set_qubit(req->targets[0]);
        getreq.__set_outcom(0);
        GetProbOfOutcomeResp getresp;
        SINGLETON(CQuSproutServerHandler)->getProbOfOutcome(getresp, getreq);
        resp->setcode(req->flowid, switchCode(getresp.base.code));
        if (getresp.base.code == ErrCode::type::COM_SUCCESS)
        {
            resp->outcomes.push_back(getresp.pro_outcome);
        }
        return;
    }

    GetProbOfAllOutcomReq getreq;
    getreq.__set_id(req->taskid);
    std::vector<int32_t> targets;
    for (auto target : req->targets)
    {
        targets.push_back(target);
    }
    getreq.__set_targets(targets);
    GetProbOfAllOutcomResp getresp;
    SINGLETON(CQuSproutServerHandler)->getProbOfAllOutcome(getresp, getreq);
    resp->setcode(req->flowid, switchCode(getresp.base.code));
    if (getresp.base.code == ErrCode::type::COM_SUCCESS)
    {
        for (auto temp : getresp.pro_outcomes)
        {
            resp->outcomes.push_back(temp);
        }
    }
}

void CReqManager::getState(const std::shared_ptr<GetStateReq> req, std::shared_ptr<GetStateResp> resp)
{
    GetAllStateReq getreq;
    getreq.__set_id(req->taskid);

    GetAllStateResp getresp;
    SINGLETON(CQuSproutServerHandler)->getAllState(getresp, getreq);
    resp->setcode(req->flowid, switchCode(getresp.base.code));

    resp->states = getresp.all_state;
}

void CReqManager::releaseEnv(const std::shared_ptr<ReleaseEnvReq> req, std::shared_ptr<ReleaseEnvResp> resp)
{
    CancelCmdReq cancelreq;
    cancelreq.__set_id(req->taskid);

    CancelCmdResp cancelresp;
    SINGLETON(CQuSproutServerHandler)->cancelCmd(cancelresp, cancelreq);
    resp->setcode(req->flowid, switchCode(cancelresp.base.code));
}

void CReqManager::getTask(const std::shared_ptr<GetTaskReq> req, std::shared_ptr<GetTaskResp> resp)
{
    GetTaskInfoReq taskreq;
    taskreq.__set_id(req->taskid);

    GetTaskInfoResp taskresp;
    SINGLETON(CQuSproutServerHandler)->getTaskInfo(taskresp, taskreq);
    resp->state = taskresp.state;
    resp->setcode(req->flowid, switchCode(taskresp.base.code));
}

void CReqManager::getMeasure(const std::shared_ptr<GetMeasureReq> req, std::shared_ptr<GetMeasureResp> resp)
{
    MeasureQubitsReq measreq;
    measreq.__set_id(req->taskid);
    std::vector<int32_t> qubits;
    for (auto target : req->targets)
    {
        qubits.push_back(target);
    }
    measreq.__set_qubits(qubits);

    MeasureQubitsResp measresp;
    SINGLETON(CQuSproutServerHandler)->measureQubits(measresp, measreq);
    resp->setcode(req->flowid, switchCode(measresp.base.code));

    for (auto temp : measresp.results)
    {
        MeasureInfo measure;
        measure.target = temp.id;
        measure.value = temp.value;
        resp->measures.push_back(measure);
    }

    for (auto temp : measresp.outcomes)
    {
        OutcomeInfo outcome;
        outcome.bitstr =  temp.bitstr;
        outcome.count =  temp.count;
        resp->outcomes.push_back(outcome);
    }
}

void CReqManager::applyQft(const std::shared_ptr<ApplyQftReq> req, std::shared_ptr<ApplyQftResp> resp)
{
    if (req->targets.size() == 0)
    {
        ApplyFullQFTReq applyreq;
        applyreq.__set_id(req->taskid);

        ApplyFullQFTResp applyresp;
        SINGLETON(CQuSproutServerHandler)->applyFullQFT(applyresp, applyreq);
        resp->setcode(req->flowid, switchCode(applyresp.base.code));
        return;
    }

    ApplyQFTReq applyreq;
    applyreq.__set_id(req->taskid);
    std::vector<int32_t>  targets;
    for (auto target : req->targets)
    {
        targets.push_back(target);
    }
    applyreq.__set_targets(targets);

    ApplyQFTResp applyresp;
    SINGLETON(CQuSproutServerHandler)->applyQFT(applyresp, applyreq);
    resp->setcode(req->flowid, switchCode(applyresp.base.code));
    return;
}

void CReqManager::getEPauli(const std::shared_ptr<GetEPauliReq> req, std::shared_ptr<GetEPauliResp> resp)
{
    GetExpecPauliProdReq getreq;
    getreq.__set_id(req->taskid);
    std::vector<PauliProdInfo> pauliprod;
    for (auto pauli : req->paulis)
    {
        PauliProdInfo temp;
        temp.__set_oper_type((PauliOperType::type)pauli.opertype);
        temp.__set_target(pauli.target);
        pauliprod.push_back(temp);
    }
    getreq.__set_pauli_prod(pauliprod);

    GetExpecPauliProdResp getresp;
    SINGLETON(CQuSproutServerHandler)->getExpecPauliProd(getresp, getreq);
    resp->expect = getresp.expect;
    resp->setcode(req->flowid, switchCode(getresp.base.code));
}

void CReqManager::getEPauliSum(const std::shared_ptr<GetEPauliSumReq> req, std::shared_ptr<GetEPauliSumResp> resp)
{
    GetExpecPauliSumReq getreq;
    getreq.__set_id(req->taskid);
    getreq.__set_term_coeff_list(req->terms);
    std::vector<PauliOperType::type> opertypes;
    for (auto opertype : req->opertypes)
    {
        opertypes.push_back((PauliOperType::type)opertype);
    }
    getreq.__set_oper_type_list(opertypes);

    GetExpecPauliSumResp getresp;
    SINGLETON(CQuSproutServerHandler)->getExpecPauliSum(getresp, getreq);
    resp->expect = getresp.expect;
    resp->setcode(req->flowid, switchCode(getresp.base.code));
}

int CReqManager::switchCode(const ErrCode::type code)
{
    switch(code)
    {
    case ErrCode::type::COM_SUCCESS:
        return COMMON_SUCCESS;
    case ErrCode::type::COM_OTHRE:
        return COMMON_OHER;
    case ErrCode::type::COM_INVALID_PARAM:
        return COMMON_INVALID_PARAM;
    case ErrCode::type::COM_TIMEOUT:
        return COMMON_TIMEOUT;
    case ErrCode::type::COM_EXCEPTION:
        return COMMON_EXCEPTION;
    case ErrCode::type::COM_NOT_EXIST:
        return COMMON_NOT_EXIST;
    case ErrCode::type::COM_IS_EXIST:
        return COMMON_IS_EXIST;
    case ErrCode::type::COM_IS_QUEUE:
        return COMMON_IS_QUEUE;
    case ErrCode::type::QUROOT_MEM_NOT_ENOUGH:
        return COMMON_MEM_NOT_ENOUGH;
    case ErrCode::type::QUROOT_NOT_INIT:
        return COMMON_NOT_INIT;
    case ErrCode::type::QUROOT_NOT_REGISTER:
        return COMMON_OHER;
    case ErrCode::type::QUROOT_NOT_RESOURCE:
        return COMMON_NOT_RESOURCE;
    case ErrCode::type::QUROOT_PRASE_ERROR:
        return COMMON_PRASE_ERROR;
    }

    return COMMON_OHER;
}
