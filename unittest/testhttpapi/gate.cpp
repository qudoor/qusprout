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

#include <fstream>
#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>
#include <hv/requests.h>
#include <hv/axios.h>
#include <hv/hthread.h>
#include "gate.h"

using namespace hv;

#define ASSERT_CODE(code, msg) \
if (code != 0) { \
    std::cout << "call " << __func__ << " failed(code:" << code << ",msg:" << msg << ")." << std::endl; \
    return false; \
}

#define ASSERT_RNT(rnt) \
if (false == rnt) { \
    return false; \
}

long long int getCurrMs()
{
    struct timeval time;
    gettimeofday(&time, NULL);
    return (time.tv_sec*1000 + time.tv_usec/1000);
}

CGate::CGate()
{

}

CGate::~CGate()
{
    releaseQCircuit();
}

bool CGate::init(const int qubitnum, const bool density, const int exectype, const int index, const std::string& ip, const int port)
{
    if (qubitnum <= 0)
    {
        return false;
    }

    m_qubitnum = qubitnum;
    m_density = density;
    m_exectype = exectype;
    m_ip = ip;
    m_port = port;
    m_taskIndex = index;
    std::ostringstream os("");

    os << getCurrMs() << "_" << m_taskIndex << "_" << m_index++;
    m_taskid = os.str();

    return createQCircuit();
}

bool CGate::createQCircuit()
{
    InitEnvReq initreq;
    InitEnvResp initresp;
    initreq.flowid = getSeq();
    initreq.taskid = m_taskid;
    initreq.params.qubits = m_qubitnum;
    initreq.params.density = m_density;
    initreq.params.exec_type = m_exectype;
    bool rnt = sendclient(initreq, initresp);
    ASSERT_RNT(rnt);
    ASSERT_CODE(initresp.code, initresp.msg);
    return true;
}

bool CGate::createPlusState()
{
    std::vector<int> targets;
    for (auto i = 0; i < m_qubitnum; ++i)
    {
        targets.push_back(i);
    }

    return hGate(targets);
}

bool CGate::getProbAmp(const int index, double& amp)
{
    GetAmpReq ampreq;
    GetAmpResp ampresp;
    ampreq.flowid = getSeq();
    ampreq.taskid = m_taskid;
    ampreq.indexs.push_back(index);
    bool rnt = sendclient(ampreq, ampresp);
    ASSERT_RNT(rnt);
    ASSERT_CODE(ampresp.code, ampresp.msg);
    if (ampresp.amps.size() > 0)
    {
        amp = ampresp.amps[0];
    }
    return true;
}

bool CGate::run(const int shots, RunGateCmdResp& runresp)
{
    RunGateCmdReq runreq;
    runreq.flowid = getSeq();
    runreq.taskid = m_taskid;
    runreq.shots = shots;
    bool rnt = sendclient(runreq, runresp);
    ASSERT_RNT(rnt);
    ASSERT_CODE(runresp.code, runresp.msg);
    m_isrelease = true;
    return true;
}

bool CGate::getProb(const std::vector<int>& targets, std::vector<double>& outcomes)
{
    GetProbReq getreq;
    GetProbResp getresp;
    getreq.flowid = getSeq();
    getreq.taskid = m_taskid;
    for (auto target : targets)
    {
        getreq.targets.push_back(target);
    }
    bool rnt = sendclient(getreq, getresp);
    ASSERT_RNT(rnt);
    ASSERT_CODE(getresp.code, getresp.msg);
    for (auto outcome : getresp.outcomes)
    {
        outcomes.push_back(outcome);
    }
    return true;
}

bool CGate::getState(std::vector<std::string>& states)
{
    GetStateReq getreq;
    GetStateResp getresp;
    getreq.flowid = getSeq();
    getreq.taskid = m_taskid;
    bool rnt = sendclient(getreq, getresp);
    ASSERT_RNT(rnt);
    ASSERT_CODE(getresp.code, getresp.msg);
    for (auto state : getresp.states)
    {
        states.push_back(state);
    }
    return true;
}

bool CGate::getTask(int& state)
{
    GetTaskReq getreq;
    GetTaskResp getresp;
    getreq.flowid = getSeq();
    getreq.taskid = m_taskid;
    bool rnt = sendclient(getreq, getresp);
    ASSERT_RNT(rnt);
    ASSERT_CODE(getresp.code, getresp.msg);
    state = getresp.state;
    return true;
}

bool CGate::getMeasure(const std::vector<int>& targets, GetMeasureResp& getresp)
{
    GetMeasureReq getreq;
    getreq.flowid = "7";
    getreq.taskid = m_taskid;
    for (auto target : targets)
    {
        getreq.targets.push_back(target);
    }
    bool rnt = sendclient(getreq, getresp);
    ASSERT_RNT(rnt);
    ASSERT_CODE(getresp.code, getresp.msg);
    return true;
}

bool CGate::applyQftReq(const std::vector<int>& targets)
{
    ApplyQftReq getreq;
    ApplyQftResp getresp;
    getreq.flowid = getSeq();
    getreq.taskid = m_taskid;
    for (auto target : targets)
    {
        getreq.targets.push_back(target);
    }
    bool rnt = sendclient(getreq, getresp);
    ASSERT_RNT(rnt);
    ASSERT_CODE(getresp.code, getresp.msg);
    return true;
}

bool CGate::getEPauli(const std::vector<PauliInfo>& paulis, double& expect)
{
    GetEPauliReq getreq;
    GetEPauliResp getresp;
    getreq.flowid = getSeq();
    getreq.taskid = m_taskid;
    for (auto pauli : paulis)
    {
        getreq.paulis.push_back(pauli);
    }
    bool rnt = sendclient(getreq, getresp);
    ASSERT_RNT(rnt);
    ASSERT_CODE(getresp.code, getresp.msg);
    expect = getresp.expect;
    return true;
}

bool CGate::getEPauliSum(const std::vector<int>& opertypes, const std::vector<double>& terms, double& expect)
{
    GetEPauliSumReq getreq;
    GetEPauliSumResp getresp;
    getreq.flowid = getSeq();
    getreq.taskid = m_taskid;
    for (auto opertype : opertypes)
    {
        getreq.opertypes.push_back(opertype);
    }
    for (auto term : terms)
    {
        getreq.terms.push_back(term);
    }
    bool rnt = sendclient(getreq, getresp);
    ASSERT_RNT(rnt);
    ASSERT_CODE(getresp.code, getresp.msg);
    expect = getresp.expect;
    return true;
}

bool CGate::releaseQCircuit()
{
    if (m_isrelease)
    {
        return true;
    }
    ReleaseEnvReq releasereq;
    ReleaseEnvResp releaseresp;
    releasereq.flowid = getSeq();
    releasereq.taskid = m_taskid;
    bool rnt = sendclient(releasereq, releaseresp);
    ASSERT_RNT(rnt);
    ASSERT_CODE(releaseresp.code, releaseresp.msg);
    m_isrelease = true;
    return true;
}

bool CGate::getMetrics(std::string& metrics)
{
    std::ostringstream os("");
    os << "http://" << m_ip << ":" << m_port << "/metrics";
    HttpClient syncclient;
    HttpRequest httpreq;
    httpreq.method = HTTP_GET;
    httpreq.url = os.str();
    httpreq.headers["Connection"] = "keep-alive";
    httpreq.timeout = 120;
    HttpResponse httpresp;
    int ret = syncclient.send(&httpreq, &httpresp);
    if (0 != ret)
    {
        std::cout << "call sendclient exception(req:" << httpresp.body << ",ret:" << ret << ")." << std::endl;
        return false;
    }
    metrics = httpresp.body;
    return true;
}

bool CGate::hGate(const std::vector<int>& targets)
{
    AddGateCmdReq addreq;
    AddGateCmdResp addresp;
    addreq.flowid = getSeq();
    addreq.taskid = m_taskid;
    addreq.params.final = 0;
    std::ostringstream os("");
    for (auto target : targets)
    {
        GateCmd cmd;
        cmd.gate = "h";
        cmd.targets.push_back(target);
        os.str("");
        os << "h q[" << target << "]";
        cmd.qasmdef = os.str();
        cmd.inverse = 0;
        addreq.params.circuits.push_back(cmd);
    }
    bool rnt = sendclient(addreq, addresp);
    ASSERT_RNT(rnt);
    ASSERT_CODE(addresp.code, addresp.msg);
    return true;
}

bool CGate::mczGate(const std::vector<int>& controls, const std::vector<int>& targets)
{
    AddGateCmdReq addreq;
    AddGateCmdResp addresp;
    addreq.flowid = getSeq();
    addreq.taskid = m_taskid;
    addreq.params.final = 0;
    std::ostringstream os(""), osdesc("");
    {
        GateCmd cmd;
        cmd.gate = "mcz";
        size_t ctlsize = controls.size();
        size_t tagsize = targets.size();
        for (size_t i = 0; i < ctlsize; i++)
        {
            cmd.controls.push_back(controls[i]);
            if (0 == i)
            {
                os << "q[" << controls[i] << "]";
            }
            else
            {
                os << ",q[" << controls[i] << "]";
            }
        }
        for (size_t i = 0; i < tagsize; i++)
        {
            cmd.targets.push_back(targets[i]);
            if (0 == ctlsize && 0 == i)
            {
                os << "q[" << targets[i] << "]";
            }
            else
            {
                os << ",q[" << targets[i] << "]";
            }
        }
        osdesc << "mcz(" << ctlsize << ") " << os.str();
        cmd.qasmdef = osdesc.str();
        cmd.inverse = 0;
        addreq.params.circuits.push_back(cmd);
    }
    bool rnt = sendclient(addreq, addresp);
    ASSERT_RNT(rnt);
    ASSERT_CODE(addresp.code, addresp.msg);
    return true;
}

bool CGate::xGate(const std::vector<int>& targets)
{
    AddGateCmdReq addreq;
    AddGateCmdResp addresp;
    addreq.flowid = getSeq();
    addreq.taskid = m_taskid;
    addreq.params.final = 0;
    std::ostringstream os("");
    for (auto target : targets)
    {
        GateCmd cmd;
        cmd.gate = "x";
        cmd.targets.push_back(target);
        os.str("");
        os << "x q[" << target << "]";
        cmd.qasmdef = os.str();
        cmd.inverse = 0;
        addreq.params.circuits.push_back(cmd);
    }
    bool rnt = sendclient(addreq, addresp);
    ASSERT_RNT(rnt);
    ASSERT_CODE(addresp.code, addresp.msg);
    return true;
}

bool CGate::cnotGate(const std::vector<int>& controls, const std::vector<int>& targets)
{
    std::ostringstream os(""), osdesc("");
    AddGateCmdReq addreq;
    AddGateCmdResp addresp;
    addreq.flowid = getSeq();
    addreq.taskid = m_taskid;
    addreq.params.final = 0;
    {
        GateCmd cmd;
        cmd.gate = "cnot";
        size_t ctlsize = controls.size();
        size_t tagsize = targets.size();
        for (size_t i = 0; i < ctlsize; i++)
        {
            cmd.controls.push_back(controls[i]);
            if (0 == i)
            {
                os << "q[" << controls[i] << "]";
            }
            else
            {
                os << ",q[" << controls[i] << "]";
            }
        }
        for (size_t i = 0; i < tagsize; i++)
        {
            cmd.targets.push_back(targets[i]);
            if (0 == ctlsize && 0 == i)
            {
                os << "q[" << targets[i] << "]";
            }
            else
            {
                os << ",q[" << targets[i] << "]";
            }
        }
        osdesc << "cnot(" << ctlsize << ") " << os.str();
        cmd.qasmdef = osdesc.str();
        cmd.inverse = 0;
        addreq.params.circuits.push_back(cmd);
    }
    bool rnt = sendclient(addreq, addresp);
    ASSERT_RNT(rnt);
    ASSERT_CODE(addresp.code, addresp.msg);
    return true;
}

bool CGate::measureGate(const std::vector<int>& targets)
{
    AddGateCmdReq addreq;
    AddGateCmdResp addresp;
    addreq.flowid = getSeq();
    addreq.taskid = m_taskid;
    addreq.params.final = 0;
    std::ostringstream os("");
    for (auto target : targets)
    {
        GateCmd cmd;
        cmd.gate = "measure";
        cmd.targets.push_back(target);
        os.str("");
        os << "measure q[" << target << "]";
        cmd.qasmdef = os.str();
        cmd.inverse = 0;
        addreq.params.circuits.push_back(cmd);
    }
    bool rnt = sendclient(addreq, addresp);
    ASSERT_RNT(rnt);
    ASSERT_CODE(addresp.code, addresp.msg);
    return true;
}

bool CGate::endAdd()
{
    AddGateCmdReq addreq;
    AddGateCmdResp addresp;
    addreq.flowid = getSeq();
    addreq.taskid = m_taskid;
    addreq.params.final = 1;
    bool rnt = sendclient(addreq, addresp);
    ASSERT_RNT(rnt);
    ASSERT_CODE(addresp.code, addresp.msg);
    return true;
}

std::string CGate::getSeq()
{
    int temp = m_seq++;
    std::ostringstream os("");
    os << temp;
    return os.str();
}

template <typename T, typename K>
bool CGate::sendclient(T& req, K& resp)
{
    std::ostringstream os("");
    os << "http://" << m_ip << ":" << m_port << "/request";
    HttpClient syncclient;
    HttpRequest httpreq;
    httpreq.method = HTTP_POST;
    httpreq.url = os.str();
    httpreq.headers["Connection"] = "keep-alive";
    httpreq.headers["Content-Type"] = "application/json";
    std::string body = "";
    req.encode(body);
    httpreq.body = body;
    httpreq.timeout = 120;
    HttpResponse httpresp;
    int ret = syncclient.send(&httpreq, &httpresp);
    if (0 != ret)
    {
        std::cout << "call sendclient exception(req:" << body << ",ret:" << ret << ")." << std::endl;
        return false;
    }
    bool rnt = resp.decode(httpresp.body);
    return rnt;
}
