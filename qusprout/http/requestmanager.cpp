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

    InitQubitsResp initresp;
    SINGLETON(CQuSproutServerHandler)->initQubits(initresp, initreq);
    resp->setcode(req->flowid, switchCode(initresp.base.code), initresp.base.msg);
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
    resp->setcode(req->flowid, switchCode(sendresp.base.code), sendresp.base.msg);
}

void CReqManager::runCmd(const std::shared_ptr<RunGateCmdReq> req, std::shared_ptr<RunGateCmdResp> resp)
{
    RunCircuitReq runreq;
    runreq.__set_id(req->taskid);
    runreq.__set_shots(req->shots);

    RunCircuitResp runresp;
    SINGLETON(CQuSproutServerHandler)->run(runresp, runreq);
    resp->setcode(req->flowid, switchCode(runresp.base.code), runresp.base.msg);

    for (auto templist : runresp.result.measures)
    {
        MeasureQus meas;
        for (auto temp : templist.measure)
        {
            MeasureQu mr;
            mr.target = temp.idx;
            mr.value = temp.value;
            meas.measure.push_back(mr);
        }
        if (meas.measure.size() > 0)
        {
            resp->measures.push_back(meas);
        }
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
        resp->setcode(req->flowid, switchCode(getresp.base.code), getresp.base.msg);
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
    resp->setcode(req->flowid, switchCode(getresp.base.code), getresp.base.msg);
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
    resp->setcode(req->flowid, switchCode(getresp.base.code), getresp.base.msg);

    resp->states = getresp.all_state;
}

void CReqManager::releaseEnv(const std::shared_ptr<ReleaseEnvReq> req, std::shared_ptr<ReleaseEnvResp> resp)
{
    CancelCmdReq cancelreq;
    cancelreq.__set_id(req->taskid);

    CancelCmdResp cancelresp;
    SINGLETON(CQuSproutServerHandler)->cancelCmd(cancelresp, cancelreq);
    resp->setcode(req->flowid, switchCode(cancelresp.base.code), cancelresp.base.msg);
}

void CReqManager::getTask(const std::shared_ptr<GetTaskReq> req, std::shared_ptr<GetTaskResp> resp)
{
    GetTaskInfoReq taskreq;
    taskreq.__set_id(req->taskid);

    GetTaskInfoResp taskresp;
    SINGLETON(CQuSproutServerHandler)->getTaskInfo(taskresp, taskreq);
    resp->state = taskresp.state;
    resp->setcode(req->flowid, switchCode(taskresp.base.code), taskresp.base.msg);
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
    resp->setcode(req->flowid, switchCode(measresp.base.code), measresp.base.msg);

    for (auto templist : measresp.result.measures)
    {
        MeasureQus meas;
        for (auto temp : templist.measure)
        {
            MeasureQu mr;
            mr.target = temp.idx;
            mr.value = temp.value;
            meas.measure.push_back(mr);
        }
        if (meas.measure.size() > 0)
        {
            resp->measures.push_back(meas);
        }
    }
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
    resp->setcode(req->flowid, switchCode(getresp.base.code), getresp.base.msg);
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
    resp->setcode(req->flowid, switchCode(getresp.base.code), getresp.base.msg);
}

void CReqManager::getRcardInfo(const std::shared_ptr<GetRCardInfoReq> req, std::shared_ptr<GetRCardInfoResp> resp)
{
    GetRandomCardInfoReq getreq;
    GetRandomCardInfoResp getresp;
    SINGLETON(CQuSproutServerHandler)->getRandomCardInfo(getresp, getreq);
    resp->count = getresp.count;
    resp->driverversion = getresp.driver_version;
    resp->libraryversion = getresp.library_version;
    for (auto temp: getresp.cards)
    {
        RCardInfo card;
        card.deviceindex = temp.device_index;
        card.mode = temp.mode;
        card.ldtemp = temp.ld_temp;
        card.bdtemp = temp.bd_temp;
        auto iter = temp.states.begin();
        for (; iter != temp.states.end(); ++iter)
        {
            RCardStateInfo cardstaate;
            cardstaate.type = (int)iter->first;
            cardstaate.state = (int)iter->second;
            card.states.push_back(cardstaate);
        }
        resp->cards.push_back(card);
    }
    resp->setcode(req->flowid, switchCode(getresp.base.code), getresp.base.msg);
}

void CReqManager::setRcard(const std::shared_ptr<SetRCardReq> req, std::shared_ptr<SetRCardResp> resp)
{
    SetRandomCardReq setreq;
    setreq.__set_device_index(req->deviceindex);
    setreq.__set_mode(req->mode);
    setreq.__set_reset(req->reset);

    SetRandomCardResp setresp;
    SINGLETON(CQuSproutServerHandler)->setRandomCard(setresp, setreq);
    resp->setcode(req->flowid, switchCode(setresp.base.code), setresp.base.msg);
}

void CReqManager::getRand(const std::shared_ptr<GetRandReq> req, std::shared_ptr<GetRandResp> resp)
{
    GetRandomReq getreq;
    getreq.__set_random_length(req->randomlength);
    getreq.__set_random_num(req->randomnum);
    if (req->deviceindex >= 0)
    {
        getreq.__set_device_index(req->deviceindex);
    }

    GetRandomResp getresp;
    SINGLETON(CQuSproutServerHandler)->getRandom(getresp, getreq);
    for (auto random : getresp.randoms)
    {
        resp->randoms.push_back(random);
    }
    resp->setcode(req->flowid, switchCode(getresp.base.code), getresp.base.msg);
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
    case ErrCode::type::COM_MEM_NOT_ENOUGH:
        return COMMON_MEM_NOT_ENOUGH;
    case ErrCode::type::COM_NOT_INIT:
        return COMMON_NOT_INIT;
    case ErrCode::type::COM_NOT_REGISTER:
        return COMMON_OHER;
    case ErrCode::type::COM_NOT_RESOURCE:
        return COMMON_NOT_RESOURCE;
    case ErrCode::type::COM_PRASE_ERROR:
        return COMMON_PRASE_ERROR;
    }

    return COMMON_OHER;
}
