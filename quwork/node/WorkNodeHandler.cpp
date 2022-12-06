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
#include "mpi.h"
#include "common/QuESTIniter.h"
#include "WorkNodeHandler.h"

CWorkNodeHandler::CWorkNodeHandler()
{
}

CWorkNodeHandler::~CWorkNodeHandler()
{
}

void CWorkNodeHandler::sendCircuitCmd(const std::vector<Cmd>& cmds, const int& final)
{
    m_Cmds = cmds;
    m_final = final;

    mpiexecute(CMDTYPE_CRICUIT);
}

void CWorkNodeHandler::getProbAmp(const int& ampindex)
{
    m_ampindex = ampindex;

    mpiexecute(CMDTYPE_PROBAMP);
}

void CWorkNodeHandler::getProbOfAllOutcome(const std::vector<int>& targets)
{
    m_targets = targets;

    mpiexecute(CMDTYPE_PROBALLOUTCOME);
}

void CWorkNodeHandler::getAllState()
{
    mpiexecute(CMDTYPE_ALLSTATE);
}

void CWorkNodeHandler::run(const int& shots)
{
    m_shots = shots;

    mpiexecute(CMDTYPE_RUN);
}

void CWorkNodeHandler::cancelCmd()
{
    mpiexecute(CMDTYPE_CANCEL);
}

void CWorkNodeHandler::getExpecPauliProd(const std::vector<PauliProdInfo>& pauli_prod)
{
    m_pauli_prod = pauli_prod;

    mpiexecute(CMDTYPE_PAULIPROD);
}

void CWorkNodeHandler::getExpecPauliSum(const GetExpecPauliSumReq& pauli_sum)
{
    m_pauli_sum = pauli_sum;

    mpiexecute(CMDTYPE_PAULISUM);
}

void CWorkNodeHandler::addCustomGateByMatrix(const GateMatrix& matrix)
{
    m_matrix = matrix;

    mpiexecute(CMDTYPE_ADDGATE);
}

void CWorkNodeHandler::resetQubits(const std::vector<int32_t>& qubits)
{
    m_reset_qubits = qubits;

    mpiexecute(CMDTYPE_RESETQUBITS);
}

void CWorkNodeHandler::getStateOfAllQubits()
{
    mpiexecute(CMDTYPE_STATEOFALL);
}

void CWorkNodeHandler::getProbabilities()
{
    mpiexecute(CMDTYPE_PROBAMPALL);
}

void CWorkNodeHandler::mpiexecute(int cmdtype)
{
    char* packbuf = new char[PACKSIZE+1];
    int  npacksize = 0;

    while(true)
    {
        MPI_Barrier(MPI_COMM_WORLD);
        memset(packbuf, '\0', PACKSIZE+1);
        npacksize = 0;

        if (SINGLETON(CQuESTIniter)->m_env.rank == 0)
        {
            packdata(cmdtype, packbuf, npacksize);
        }

        MPI_Bcast( &npacksize, 1, MPI_INT, 0, MPI_COMM_WORLD );
        MPI_Bcast( packbuf, npacksize, MPI_PACKED, 0, MPI_COMM_WORLD );

        if (SINGLETON(CQuESTIniter)->m_env.rank != 0)
        {
            NodeData nodedata;
            unpackdata(nodedata, packbuf, npacksize);

            if (nodedata.m_cmdtype != CMDTYPE_CANCEL)
            {
                execute(nodedata);
                continue;
            }
        }

        break;
    }
	delete [] packbuf;
	packbuf = NULL;
}

void CWorkNodeHandler::packdata(int cmdtype, char* packbuf, int& packsize)
{
    NodeData data;
    switch (cmdtype)
    {
        case CMDTYPE_CRICUIT:
            data.packcircuit(m_Cmds, m_final, packbuf, packsize);
            break;
        case CMDTYPE_PROBAMP:
            data.packprobamp(m_ampindex, packbuf, packsize);
            break;
        case CMDTYPE_PROBALLOUTCOME:
            data.packproballoutcome(m_targets, packbuf, packsize);
            break;
        case CMDTYPE_ALLSTATE:
            data.packallstate(packbuf, packsize);
            break;
        case CMDTYPE_RUN:
            data.packrun(m_shots, packbuf, packsize);
            break;
        case CMDTYPE_CANCEL:
            data.packcancel(packbuf, packsize);
            break;
        case CMDTYPE_PAULIPROD:
            data.packpauliprod(m_pauli_prod, packbuf, packsize);
            break;
        case CMDTYPE_PAULISUM:
            data.packpaulisum(m_pauli_sum, packbuf, packsize);
            break;
        case CMDTYPE_ADDGATE:
            data.packaddgate(m_matrix, packbuf, packsize);
            break;
        case CMDTYPE_RESETQUBITS:
            data.packresetqubits(m_reset_qubits, packbuf, packsize);
            break;
        case CMDTYPE_STATEOFALL:
            data.packstateofall(packbuf, packsize);
            break;
        case CMDTYPE_PROBAMPALL:
            data.packprobampall(packbuf, packsize);
            break;
        default:
            assert(false);
            break;
    }
}

void CWorkNodeHandler::unpackdata(NodeData& nodedata, char* packbuf, int& packsize)
{
    nodedata.unpackcmdtype(packbuf, packsize);
    switch (nodedata.m_cmdtype)
    {
        case CMDTYPE_CRICUIT:
            nodedata.unpackcircuit(packbuf, packsize);
            break;
        case CMDTYPE_PROBAMP:
            nodedata.unpackprobamp(packbuf, packsize);
            break;
        case CMDTYPE_PROBALLOUTCOME:
            nodedata.unpackproballoutcome(packbuf, packsize);
            break;
        case CMDTYPE_ALLSTATE:
            nodedata.unpackallstate(packbuf, packsize);
            break;
        case CMDTYPE_RUN:
            nodedata.unpackrun(packbuf, packsize);
            break;
        case CMDTYPE_CANCEL:
            nodedata.unpackcancel(packbuf, packsize);
            break;
        case CMDTYPE_PAULIPROD:
            nodedata.unpackpauliprod(packbuf, packsize);
            break;
        case CMDTYPE_PAULISUM:
            nodedata.unpackpaulisum(packbuf, packsize);
            break;
        case CMDTYPE_ADDGATE:
            nodedata.unpackaddgate(packbuf, packsize);
            break;
        case CMDTYPE_RESETQUBITS:
            nodedata.unpackresetqubits(packbuf, packsize);
            break;
        case CMDTYPE_STATEOFALL:
            nodedata.unpackstateofall(packbuf, packsize);
            break;
        case CMDTYPE_PROBAMPALL:
            nodedata.unpackprobampall(packbuf, packsize);
            break;
        default:
            assert(false);
            break;
    }
}

void CWorkNodeHandler::execute(NodeData& nodedata)
{
    MeasureResult result;
    Circuit circuit;
    std::vector<double> dresult;
    std::vector<std::string> sresult;
    std::vector<double> real;
    std::vector<double> imag;
    std::vector<double> probabilities;

    switch (nodedata.m_cmdtype)
    {
    case CMDTYPE_CRICUIT:
        circuit.__set_cmds(nodedata.m_Cmds);
        m_executor.receiveCircuit(circuit, nodedata.m_final);
        break;
    case CMDTYPE_PROBAMP:
        m_executor.getProbOfAmp(nodedata.m_ampindex);
        break;
    case CMDTYPE_PROBALLOUTCOME:
        m_executor.getProbOfAllOutcome(dresult, nodedata.m_targets);
        break;
    case CMDTYPE_ALLSTATE:
        m_executor.getAllState(sresult);
        break;
    case CMDTYPE_RUN:
        m_executor.run(result, nodedata.m_shots);
        break;
    case CMDTYPE_PAULIPROD:
        m_executor.getExpecPauliProd(nodedata.m_pauli_prod);
        break;
    case CMDTYPE_PAULISUM:
        m_executor.getExpecPauliSum(nodedata.m_pauli_sum.oper_type_list, nodedata.m_pauli_sum.term_coeff_list);
        break;
    case CMDTYPE_ADDGATE:
        m_executor.addCustomGateByMatrix(nodedata.m_matrix);
        break;
    case CMDTYPE_RESETQUBITS:
        m_executor.resetQubits(nodedata.m_reset_qubits);
        break;
    case CMDTYPE_STATEOFALL:
        m_executor.getStateOfAllQubits(real, imag);
        break;
    case CMDTYPE_PROBAMPALL:
        m_executor.getProbabilities(probabilities);
        break;
    default:
        assert(false);
        break;
    }
}

void CWorkNodeHandler::Print(const std::vector<Cmd>& cmds, const int& final, const int& packsize)
{
    std::stringstream ss;
    ss << "rank=" << SINGLETON(CQuESTIniter)->m_env.rank; 
    ss << " cmdnumber=" << cmds.size();
    ss << " packsize=" << packsize;
    ss << " final=" << final << std::endl;

    for(auto cmd : cmds)
    {
        ss << "gate=" << cmd.gate;

        ss << " contorl={";
        for(auto i : cmd.controls)
            ss << i << ",";
        ss << "}";

        ss << " target={";
        for(auto i : cmd.targets)
            ss << i << ",";
        ss << "}";

        ss << " rotation={";
        for(auto i : cmd.rotation)
            ss << i << ",";
        ss << "}";

        ss << " desc=" << cmd.desc << std::endl;
    }

    std::cout << ss.str();
}
