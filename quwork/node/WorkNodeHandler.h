#ifndef WorkNodeHandler_H
#define WorkNodeHandler_H

#include "node/NodeData.h"
#include "command/executor.h"

class CWorkNodeHandler
{
public:
    CWorkNodeHandler();
    virtual ~CWorkNodeHandler();

    void mpiexecute(int cmdtype = 0);

    void sendCircuitCmd(const std::vector<Cmd>& cmds, const int& final);
    void getProbAmp(const int& ampindex);
    void getProbOutcome(const int& qubit, const int& outcome);
    void getProbOfAllOutcome(const std::vector<int>& targets);
    void getAllState();
    void applyQFT(const std::vector<int>& targets);
    void applyFullQFT();
    void run(const int& shots);
    void cancelCmd();
    
    void getExpecPauliProd(const std::vector<PauliProdInfo>& pauli_prod);
    void getExpecPauliSum(const GetExpecPauliSumReq& pauli_sum);

    void addCustomGateByMatrix(const GateMatrix& matrix);
    void resetQubits(const std::vector<int32_t>& qubits);
    void getStateOfAllQubits();
    void getProbabilities();

private:
    QS::CmdExecutor m_executor;

    std::vector<Cmd> m_Cmds;
    std::vector<int> m_targets;
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

    void packdata(int cmdtype, char* packbuf, int& packsize);
    void unpackdata(NodeData& nodedata, char* packbuf, int& packsize);
    void execute(NodeData& nodedata);
    void Print(const std::vector<Cmd>& cmds, const int& final, const int& packsize);
};

#endif