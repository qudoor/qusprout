#ifndef NodeData_H
#define NodeData_H

#include <vector>
#include <string>
#include "../interface/qusproutdata_types.h"

const int PACKSIZE = 1024 * 1024;

#define CMDTYPE_CRICUIT         (1 << 0)
#define CMDTYPE_PROBAMP         (1 << 1)
#define CMDTYPE_PROBOUTCOME     (1 << 2)
#define CMDTYPE_PROBALLOUTCOME  (1 << 3)
#define CMDTYPE_ALLSTATE        (1 << 4)
#define CMDTYPE_APPLYQFT        (1 << 5)
#define CMDTYPE_APPLYFULLQFT    (1 << 6)
#define CMDTYPE_RUN             (1 << 7)
#define CMDTYPE_CANCEL          (1 << 8)
#define CMDTYPE_PAULIPROD       (1 << 9)
#define CMDTYPE_PAULISUM        (1 << 10)
#define CMDTYPE_ADDGATE         (1 << 11)
#define CMDTYPE_RESETQUBITS     (1 << 12)
#define CMDTYPE_STATEOFALL      (1 << 13)
#define CMDTYPE_PROBAMPALL      (1 << 14)
#define CMDTYPE_SETAMPLITUDES   (1 << 15)

class NodeData
{
public:
    NodeData();
    virtual ~NodeData();

    void packcmdtype(const int& cmdtype, char* packbuf, int& packsize);
    void unpackcmdtype(char* packbuf, int& packsize);

    void packcircuit(const std::vector<Cmd>& cmds, const int& final, char* packbuf, int& packsize);
    void unpackcircuit(char* packbuf, int& packsize);

    void packprobamp(const int& ampindex, char* packbuf, int& packsize);
    void unpackprobamp(char* packbuf, int& packsize);

    void packproboutcome(const int& qubit, const int& outcome, char* packbuf, int& packsize);
    void unpackproboutcome(char* packbuf, int& packsize);

    void packproballoutcome(const std::vector<int>& targets, char* packbuf, int& packsize);
    void unpackproballoutcome(char* packbuf, int& packsize);

    void packallstate(char* packbuf, int& packsize);
    void unpackallstate(char* packbuf, int& packsize);

    void packapplyqft(const std::vector<int>& targets, char* packbuf, int& packsize);
    void unpackapplyqft(char* packbuf, int& packsize);

    void packapplyfullqft(char* packbuf, int& packsize);
    void unpackapplyfullqft(char* packbuf, int& packsize);

    void packrun(const int& shots, char* packbuf, int& packsize);
    void unpackrun(char* packbuf, int& packsize);

    void packcancel(char* packbuf, int& packsize);
    void unpackcancel(char* packbuf, int& packsize);

    void packpauliprod(const std::vector<PauliProdInfo>& pauliprod, char* packbuf, int& packsize);
    void unpackpauliprod(char* packbuf, int& packsize);

    void packpaulisum(const GetExpecPauliSumReq& paulisum, char* packbuf, int& packsize);
    void unpackpaulisum(char* packbuf, int& packsize);

    void packaddgate(const GateMatrix& matrix, char* packbuf, int& packsize);
    void unpackaddgate(char* packbuf, int& packsize);

    void packresetqubits(const std::vector<int32_t>& qubits, char* packbuf, int& packsize);
    void unpackresetqubits(char* packbuf, int& packsize);

    void packstateofall(char* packbuf, int& packsize);
    void unpackstateofall(char* packbuf, int& packsize);

    void packprobampall(char* packbuf, int& packsize);
    void unpackprobampall(char* packbuf, int& packsize);

    void packamplitudes(const std::vector<double>& reals, const std::vector<double>& imags, char* packbuf, int& packsize);
    void unpackamplitudes(char* packbuf, int& packsize);
    
    std::vector<Cmd> m_Cmds;
    std::vector<int> m_targets;
    int m_cmdtype;
    int m_final;
    int m_ampindex;
    int m_qubit;
    int m_outcome;
    int m_shots;

    std::vector<PauliProdInfo> m_pauli_prod;
    GetExpecPauliSumReq m_pauli_sum;
    GateMatrix m_matrix;
    std::vector<int32_t> m_reset_qubits;

    std::vector<double> m_reals;
    std::vector<double> m_imags;
    
private:
    int m_position;

    void packstring(const std::string& str, char* packbuf, int& packsize);
    void packint(const int& pint, char* packbuf, int& packsize);
    void packvectorint(const std::vector<int>& vec, char* packbuf, int& packsize);
    void packvectordouble(const std::vector<double>& vec, char* packbuf, int& packsize);

    void unpackstring(std::string& str, char* packbuf, int& packsize);
    void unpackint(int& pint, char* packbuf, int& packsize);
    void unpackvectorint(std::vector<int>& vec, char* packbuf, int& packsize);
    void unpackvectordouble(std::vector<double>& vec, char* packbuf, int& packsize);

    void packdouble(const double& pdouble, char* packbuf, int& packsize);
    void unpackdouble(double& pdouble, char* packbuf, int& packsize);
};

#endif