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

#include <stdlib.h>
#include <math.h>
#include <sys/time.h>
#include "gate.h"

#define CALL_WITH_TRY_SERVICE(CALL, req) \
try { \
    CALL; \
} catch (const TTransportException& e) { \
    std::cout << "call exception(req:" << getPrint(req) << ",err:" << e.what() << ")." << std::endl; \
    return false; \
} catch (...) { \
    std::cout << "call other exception(req:" << getPrint(req) << ")." << std::endl; \
    return false; \
}

#define ASSERT_CODE(code, msg) \
if (code != ErrCode::type::COM_SUCCESS) { \
    std::cout << "call " << __func__ << " failed(code:" << code << ",msg:" << msg << ")." << std::endl; \
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
    if (m_transport != nullptr)
    {
        releaseQCircuit();
        m_transport->close();
        m_transport = nullptr;
    }
}

bool CGate::init(const int qubitnum, const bool density, const std::string& ip, const int port)
{
    if (qubitnum <= 0)
    {
        return false;
    }

    m_qubitnum = qubitnum;
    m_density = density;
    std::ostringstream os("");
    os << getCurrMs() << "_" << m_index++;
    m_taskid = os.str();

    try
    {
        std::shared_ptr<TSocket> socket = std::make_shared<TSocket>(ip, port); 
        m_transport = std::make_shared<TBufferedTransport>(socket);
        std::shared_ptr<TProtocol> protocol = std::make_shared<TBinaryProtocol>(m_transport);
        std::shared_ptr<TMultiplexedProtocol> quest = std::make_shared<TMultiplexedProtocol>(protocol, "QuSproutServer");
        m_transport->open();
        m_client = std::make_shared<QuSproutServerClient>(quest);
    } 
    catch (const TTransportException& e) 
    { 
        std::cout << "init exception(err:" << e.what() << ")." << std::endl;
        return false;
    } 
    catch (...) 
    {
        std::cout << "init exception." << std::endl;
        return false;
    } 

    return createQCircuit();
}

bool CGate::createQCircuit()
{
    InitQubitsReq initreq;
    initreq.__set_id(m_taskid);
    initreq.__set_qubits(m_qubitnum);
    initreq.__set_density(m_density);
    initreq.__set_exec_type(ExecCmdType::type::ExecTypeCpuSingle);
    InitQubitsResp initresp;
    CALL_WITH_TRY_SERVICE(m_client->initQubits(initresp, initreq), initreq);
    ASSERT_CODE(initresp.base.code, initresp.base.msg);
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
    GetProbAmpReq ampreq;
    ampreq.__set_id(m_taskid);
    ampreq.__set_index(index);
    GetProbAmpResp ampresp;
    CALL_WITH_TRY_SERVICE(m_client->getProbAmp(ampresp, ampreq), ampreq);
    ASSERT_CODE(ampresp.base.code, ampresp.base.msg);
    amp = ampresp.amp;
    return true;
}

bool CGate::run(const int shots, Result& result)
{
    RunCircuitReq runreq;
    RunCircuitResp runresp;
    runreq.__set_id(m_taskid);
    runreq.__set_shots(shots);
    CALL_WITH_TRY_SERVICE(m_client->run(runresp, runreq), runreq);
    ASSERT_CODE(runresp.base.code, runresp.base.msg);
    result.__set_measureSet(runresp.result.measureSet);
    result.__set_outcomeSet(runresp.result.outcomeSet);
    m_isrelease = true;
    return true;
}

bool CGate::releaseQCircuit()
{
    if (m_isrelease)
    {
        return true;
    }

    if (m_client)
    {
        CancelCmdReq cancelreq;
        cancelreq.__set_id(m_taskid);
        CancelCmdResp cancelresp;
        CALL_WITH_TRY_SERVICE(m_client->cancelCmd(cancelresp, cancelreq), cancelreq);
        ASSERT_CODE(cancelresp.base.code, cancelresp.base.msg);
    }
    
	m_isrelease = true;
    
    return true;
}

bool CGate::hGate(const std::vector<int>& targets)
{
    std::ostringstream os("");
    SendCircuitCmdReq cmdreq;
    cmdreq.__set_id(m_taskid);
    Circuit circuit;
    for (auto target : targets)
    {
        Cmd cmd;
        cmd.__set_gate("h");
        cmd.targets.push_back(target);
        os.str("");
        os << "h q[" << target << "]";
        cmd.__set_desc(os.str());
        cmd.__set_inverse(false);
        circuit.cmds.push_back(cmd);
    }
    cmdreq.__set_circuit(circuit);
    cmdreq.__set_final(false);
    SendCircuitCmdResp cmdresp;
    CALL_WITH_TRY_SERVICE(m_client->sendCircuitCmd(cmdresp, cmdreq), cmdreq);
    ASSERT_CODE(cmdresp.base.code, cmdresp.base.msg);
    return true;
}

bool CGate::mczGate(const std::vector<int>& controls, const std::vector<int>& targets)
{
    std::ostringstream os(""), osdesc("");
    SendCircuitCmdReq cmdreq;
    cmdreq.__set_id(m_taskid);
    Circuit circuit;
    {
        Cmd cmd;
        cmd.__set_gate("mcz");
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
        cmd.__set_desc(osdesc.str());
        cmd.__set_inverse(false);
        circuit.cmds.push_back(cmd);
    }
    cmdreq.__set_circuit(circuit);
    cmdreq.__set_final(false);
    SendCircuitCmdResp cmdresp;
    CALL_WITH_TRY_SERVICE(m_client->sendCircuitCmd(cmdresp, cmdreq), cmdreq);
    ASSERT_CODE(cmdresp.base.code, cmdresp.base.msg);
    return true;
}

bool CGate::xGate(const std::vector<int>& targets)
{
    std::ostringstream os("");
    SendCircuitCmdReq cmdreq;
    cmdreq.__set_id(m_taskid);
    Circuit circuit;
    for (auto target : targets)
    {
        Cmd cmd;
        cmd.__set_gate("x");
        cmd.targets.push_back(target);
        os.str("");
        os << "x q[" << target << "]";
        cmd.__set_desc(os.str());
        cmd.__set_inverse(false);
        circuit.cmds.push_back(cmd);
    }
    cmdreq.__set_circuit(circuit);
    cmdreq.__set_final(false);
    SendCircuitCmdResp cmdresp;
    CALL_WITH_TRY_SERVICE(m_client->sendCircuitCmd(cmdresp, cmdreq), cmdreq);
    ASSERT_CODE(cmdresp.base.code, cmdresp.base.msg);
    return true;
}

bool CGate::cnotGate(const std::vector<int>& controls, const std::vector<int>& targets)
{
    std::ostringstream os(""), osdesc("");
    SendCircuitCmdReq cmdreq;
    cmdreq.__set_id(m_taskid);
    Circuit circuit;
    {
        Cmd cmd;
        cmd.__set_gate("cnot");
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
        cmd.__set_desc(osdesc.str());
        cmd.__set_inverse(false);
        circuit.cmds.push_back(cmd);
    }
    cmdreq.__set_circuit(circuit);
    cmdreq.__set_final(false);
    SendCircuitCmdResp cmdresp;
    CALL_WITH_TRY_SERVICE(m_client->sendCircuitCmd(cmdresp, cmdreq), cmdreq);
    ASSERT_CODE(cmdresp.base.code, cmdresp.base.msg);
    return true;
}

bool CGate::measureGate(const std::vector<int>& targets)
{
    std::ostringstream os("");
    SendCircuitCmdReq cmdreq;
    cmdreq.__set_id(m_taskid);
    Circuit circuit;
    for (auto target : targets)
    {
        Cmd cmd;
        cmd.__set_gate("measure");
        cmd.targets.push_back(target);
        os.str("");
        os << "measure q[" << target << "]";
        cmd.__set_desc(os.str());
        cmd.__set_inverse(false);
        circuit.cmds.push_back(cmd);
    }
    cmdreq.__set_circuit(circuit);
    cmdreq.__set_final(false);
    SendCircuitCmdResp cmdresp;
    CALL_WITH_TRY_SERVICE(m_client->sendCircuitCmd(cmdresp, cmdreq), cmdreq);
    ASSERT_CODE(cmdresp.base.code, cmdresp.base.msg);
    return true;
}

bool CGate::endAdd()
{
    std::ostringstream os("");
    SendCircuitCmdReq cmdreq;
    cmdreq.__set_id(m_taskid);
    cmdreq.__set_final(true);
    SendCircuitCmdResp cmdresp;
    CALL_WITH_TRY_SERVICE(m_client->sendCircuitCmd(cmdresp, cmdreq), cmdreq);
    ASSERT_CODE(cmdresp.base.code, cmdresp.base.msg);
    return true;
}
