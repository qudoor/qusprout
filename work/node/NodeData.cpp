#include <iostream>
#include <sstream>
#include <memory>

#include "mpi.h"

#include "../../common/qulog.h"

#include "NodeData.h"

NodeData::NodeData()
{
}

NodeData::~NodeData()
{
}

void NodeData::packcmdtype(const int& cmdtype, char* packbuf, int& packsize)
{
    //pack command type
    packint(cmdtype, packbuf, packsize);
}

void NodeData::unpackcmdtype(char* packbuf, int& packsize)
{
    m_position = 0;

    //unpack command type
    unpackint(m_cmdtype, packbuf, packsize);
}

void NodeData::packcircuit(const std::vector<Cmd>& cmds, const int& final, char* packbuf, int& packsize)
{
    //pack command type
    packcmdtype(CMDTYPE_CRICUIT, packbuf, packsize);

    //pack final
    packint(final, packbuf, packsize);

    //pack circuit
    packint(cmds.size(), packbuf, packsize);
    for(auto& cmd : cmds)
    {
        packstring(cmd.gate, packbuf, packsize);
        packvectorint(cmd.controls, packbuf, packsize);
        packvectorint(cmd.targets, packbuf, packsize);
        packvectordouble(cmd.rotation, packbuf, packsize);
        packstring(cmd.desc, packbuf, packsize);
    }
}

void NodeData::unpackcircuit(char* packbuf, int& packsize)
{
    //unpack final
    unpackint(m_final, packbuf, packsize);

    //unpack circuit
    int cmdsize = 0;
    unpackint(cmdsize, packbuf, packsize);
    for(int i = 0; i < cmdsize; i++)
    {
        Cmd cmd;
        unpackstring(cmd.gate, packbuf, packsize);
        unpackvectorint(cmd.controls, packbuf, packsize);
        unpackvectorint(cmd.targets, packbuf, packsize);
        unpackvectordouble(cmd.rotation, packbuf, packsize);
        unpackstring(cmd.desc, packbuf, packsize);

        m_Cmds.push_back(cmd);
    }
}

void NodeData::packprobamp(const int& ampindex, char* packbuf, int& packsize)
{
    //pack command type
    packcmdtype(CMDTYPE_PROBAMP, packbuf, packsize);

    //pack amp index
    packint(ampindex, packbuf, packsize);
}

void NodeData::unpackprobamp(char* packbuf, int& packsize)
{
    //unpack amp index
    unpackint(m_ampindex, packbuf, packsize);
}

void NodeData::packproboutcome(const int& qubit, const int& outcome, char* packbuf, int& packsize)
{
    //pack command type
    packcmdtype(CMDTYPE_PROBOUTCOME, packbuf, packsize);

    //pack qubit
    packint(qubit, packbuf, packsize);

    //pack outcome
    packint(outcome, packbuf, packsize);
}

void NodeData::unpackproboutcome(char* packbuf, int& packsize)
{
    //unpack qubit
    unpackint(m_qubit, packbuf, packsize);

    //unpack outcome
    unpackint(m_outcome, packbuf, packsize);
}

void NodeData::packproballoutcome(const std::vector<int>& targets, char* packbuf, int& packsize)
{
    //pack command type
    packcmdtype(CMDTYPE_PROBALLOUTCOME, packbuf, packsize);

    //pack targets
    packvectorint(targets, packbuf, packsize);
}

void NodeData::unpackproballoutcome(char* packbuf, int& packsize)
{
    //unpack targets
    unpackvectorint(m_targets, packbuf, packsize);
}

void NodeData::packallstate(char* packbuf, int& packsize)
{
    //pack command type
    packcmdtype(CMDTYPE_ALLSTATE, packbuf, packsize);
}

void NodeData::unpackallstate(char* packbuf, int& packsize)
{
}

void NodeData::packapplyqft(const std::vector<int>& targets, char* packbuf, int& packsize)
{
    //pack command type
    packcmdtype(CMDTYPE_APPLYQFT, packbuf, packsize);

    //pack targets
    packvectorint(targets, packbuf, packsize);
}

void NodeData::unpackapplyqft(char* packbuf, int& packsize)
{
    //unpack targets
    unpackvectorint(m_targets, packbuf, packsize);
}

void NodeData::packapplyfullqft(char* packbuf, int& packsize)
{
    //pack command type
    packcmdtype(CMDTYPE_APPLYFULLQFT, packbuf, packsize);
}

void NodeData::unpackapplyfullqft(char* packbuf, int& packsize)
{
}

void NodeData::packrun(const int& shots, char* packbuf, int& packsize)
{
    //pack command type
    packcmdtype(CMDTYPE_RUN, packbuf, packsize);

    //pack shots
    packint(shots, packbuf, packsize);
}

void NodeData::unpackrun(char* packbuf, int& packsize)
{
    //unpack shots
    unpackint(m_shots, packbuf, packsize);
}

void NodeData::packcancel(char* packbuf, int& packsize)
{
    //pack command type
    packcmdtype(CMDTYPE_CANCEL, packbuf, packsize);
}

void NodeData::unpackcancel(char* packbuf, int& packsize)
{
}

void NodeData::packpauliprod(const std::vector<PauliProdInfo>& pauliprod, char* packbuf, int& packsize)
{
    //pack command type
    packcmdtype(CMDTYPE_PAULIPROD, packbuf, packsize);

    //pack size
    packint(pauliprod.size(), packbuf, packsize);
    for(auto& temp : pauliprod)
    {
        packint((int)temp.oper_type, packbuf, packsize);
        packint(temp.target, packbuf, packsize);
    }
}

void NodeData::unpackpauliprod(char* packbuf, int& packsize)
{
    //unpack size
    int paulisize = 0;
    unpackint(paulisize, packbuf, packsize);
    for(int i = 0; i < paulisize; i++)
    {
        PauliProdInfo temp;
        int optype = 0;
        unpackint(optype, packbuf, packsize);
        unpackint(temp.target, packbuf, packsize);
        temp.oper_type = (PauliOperType::type)optype;

        m_pauli_prod.push_back(temp);
    }
}

void NodeData::packpaulisum(const GetExpecPauliSumReq& paulisum, char* packbuf, int& packsize)
{
    //pack command type
    packcmdtype(CMDTYPE_PAULISUM, packbuf, packsize);

    //pack size
    packint(paulisum.oper_type_list.size(), packbuf, packsize);
    for(auto& temp : paulisum.oper_type_list)
    {
        packint((int)temp, packbuf, packsize);
    }

    packint(paulisum.term_coeff_list.size(), packbuf, packsize);
    for(auto& temp : paulisum.term_coeff_list)
    {
        packdouble(temp, packbuf, packsize);
    }
}

void NodeData::unpackpaulisum(char* packbuf, int& packsize)
{
    //unpack size
    int typesize = 0;
    unpackint(typesize, packbuf, packsize);
    std::vector<PauliOperType::type> typetemp;
    for(int i = 0; i < typesize; i++)
    {
        int optype = 0;
        unpackint(optype, packbuf, packsize);
        PauliOperType::type oper_type = (PauliOperType::type)optype;
        typetemp.push_back(oper_type);
    }
    m_pauli_sum.__set_oper_type_list(typetemp);

    int coeffisize = 0;
    unpackint(coeffisize, packbuf, packsize);
    std::vector<double> coeffitemp;
    for(int i = 0; i < coeffisize; i++)
    {
        double coeffi = 0;
        unpackdouble(coeffi, packbuf, packsize);

        coeffitemp.push_back(coeffi);
    }
    m_pauli_sum.__set_term_coeff_list(coeffitemp);
}

void NodeData::packstring(const std::string& str, char* packbuf, int& packsize)
{
    int size = str.size();
    MPI_Pack( &size, 1, MPI_INT, packbuf, PACKSIZE, &packsize, MPI_COMM_WORLD );
    MPI_Pack( str.c_str(), size, MPI_CHAR, packbuf, PACKSIZE, &packsize, MPI_COMM_WORLD );
}

void NodeData::packint(const int& pint, char* packbuf, int& packsize)
{
    MPI_Pack( &pint, 1, MPI_INT, packbuf, PACKSIZE, &packsize, MPI_COMM_WORLD );
}

void NodeData::packvectorint(const std::vector<int>& vec, char* packbuf, int& packsize)
{
    int size = vec.size();
    MPI_Pack( &size, 1, MPI_INT, packbuf, PACKSIZE, &packsize, MPI_COMM_WORLD );
    for(auto i : vec)
        MPI_Pack( &i, 1, MPI_INT, packbuf, PACKSIZE, &packsize, MPI_COMM_WORLD );
}

void NodeData::packvectordouble(const std::vector<double>& vec, char* packbuf, int& packsize)
{
    int size = vec.size();
    MPI_Pack( &size, 1, MPI_INT, packbuf, PACKSIZE, &packsize, MPI_COMM_WORLD );
    for(auto i : vec)
        MPI_Pack( &i, 1, MPI_DOUBLE, packbuf, PACKSIZE, &packsize, MPI_COMM_WORLD );
}

void NodeData::unpackstring(std::string& str, char* packbuf, int& packsize)
{
    int size = 0;
    MPI_Unpack( packbuf, packsize, &m_position, &size, 1, MPI_INT, MPI_COMM_WORLD );

    std::unique_ptr<char[]> autostr(new char[size+1]{0});
    MPI_Unpack( packbuf, packsize, &m_position, (void*)autostr.get(), size, MPI_CHAR, MPI_COMM_WORLD );

    str = autostr.get();
}

void NodeData::unpackint(int& pint, char* packbuf, int& packsize)
{
    MPI_Unpack( packbuf, packsize, &m_position, &pint, 1, MPI_INT, MPI_COMM_WORLD );
}

void NodeData::unpackvectorint(std::vector<int>& vec, char* packbuf, int& packsize)
{
    int size = 0;
    MPI_Unpack( packbuf, packsize, &m_position, &size, 1, MPI_INT, MPI_COMM_WORLD );
    for(int i = 0; i < size; i++)
    {
        int controlbit = 0;
        MPI_Unpack( packbuf, packsize, &m_position, &controlbit, 1, MPI_INT, MPI_COMM_WORLD );
        vec.push_back(controlbit);
    }    
}

void NodeData::unpackvectordouble(std::vector<double>& vec, char* packbuf, int& packsize)
{
    int size = 0;
    MPI_Unpack( packbuf, packsize, &m_position, &size, 1, MPI_INT, MPI_COMM_WORLD );
    for(int i = 0; i < size; i++)
    {
        double rotationbit = 0;
        MPI_Unpack( packbuf, packsize, &m_position, &rotationbit, 1, MPI_DOUBLE, MPI_COMM_WORLD );
        vec.push_back(rotationbit);
    }
}

void NodeData::packdouble(const double& pdouble, char* packbuf, int& packsize)
{
    MPI_Pack( &pdouble, 1, MPI_DOUBLE, packbuf, PACKSIZE, &packsize, MPI_COMM_WORLD );
}

void NodeData::unpackdouble(double& pdouble, char* packbuf, int& packsize)
{
    MPI_Unpack( packbuf, packsize, &m_position, &pdouble, 1, MPI_DOUBLE, MPI_COMM_WORLD );
}