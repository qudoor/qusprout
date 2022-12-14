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

#ifndef EXECUTOR_H
#define EXECUTOR_H

#include <vector>
#include <mutex>
#include <atomic>
#include "QuEST.h"
#include "common/general.h"
#include "interface/qusproutdata_types.h"

BEGIN_NAMESPACE(QS)

class CmdExecutor {
public:
    CmdExecutor();
    ~CmdExecutor();

public:
    void receiveCircuit(const ::Circuit& circuit, const bool final);
    void run(MeasureResult& result, int32_t shots);
    double getProbOfAmp(const int64_t index);
    double getProbOfOutcome(const int32_t qubit, const int32_t outcome);
    void getProbOfAllOutcome(std::vector<double> & _return, const std::vector<int32_t> & qubits);
    void getAllState(std::vector<std::string> & _return, const std::string& id = "");
    void packMeasureResult();
    void printCmd(const Cmd& cmd);
    bool isInit();

    double getExpecPauliProd(const std::vector<PauliProdInfo>& pauli_prod);
    double getExpecPauliSum(const std::vector<PauliOperType::type>& oper_type_list, const std::vector<double>& term_coeff_list);

    void getMeasureResult(const std::vector<int32_t>& qubits, MeasureResult& result);
    int addCustomGateByMatrix(const GateMatrix& matrix);
    int appendQubits(const int add_qubits);
    void reInit(const int qubits);
    void resetQubits(const std::vector<int32_t>& qubits);

    void getStateOfAllQubits(std::vector<double>& real, std::vector<double>& imag);
    void getProbabilities(std::vector<double>& probabilities);

    long long int getMaxAmpIndex();

public:
    void execCmd(const Cmd& cmd);
    void H(const Cmd& cmd);
    void Measure(const Cmd& cmd);
    void barrier(const Cmd& cmd);
    void P(const Cmd& cmd);
    void CP(const Cmd& cmd);
    void R(const Cmd& cmd);
    void X(const Cmd& cmd);
    void Y(const Cmd& cmd);
    void Z(const Cmd& cmd);
    void Rx(const Cmd& cmd);
    void Ry(const Cmd& cmd);
    void Rz(const Cmd& cmd);
    void Rxx(const Cmd& cmd);
    void Ryy(const Cmd& cmd);
    void Rzz(const Cmd& cmd);
    void S(const Cmd& cmd);
    void T(const Cmd& cmd);
    void Sdg(const Cmd& cmd);
    void Tdg(const Cmd& cmd);
    void SqrtX(const Cmd& cmd);
    void Swap(const Cmd& cmd);
    void SqrtSwap(const Cmd& cmd);
    void CNOT(const Cmd& cmd);
    void CY(const Cmd& cmd);
    void CZ(const Cmd& cmd);
    void U3(const Cmd& cmd);
    void U2(const Cmd& cmd);
    void U1(const Cmd& cmd);
    void CRx(const Cmd& cmd);
    void CRy(const Cmd& cmd);
    void CRz(const Cmd& cmd);
    void X1(const Cmd& cmd);
    void Y1(const Cmd& cmd);
    void Z1(const Cmd& cmd);
    void CU1(const Cmd& cmd);
    void CU3(const Cmd& cmd);
    void U(const Cmd& cmd);
    void CU(const Cmd& cmd);
    void CR(const Cmd& cmd);
    void iSwap(const Cmd& cmd);
    void id(const Cmd& cmd);
    void CH(const Cmd& cmd);
    void SqrtXdg(const Cmd& cmd);
    void CSqrtX(const Cmd& cmd);
    void CSwap(const Cmd& cmd);
    void MatrixN(const Cmd& cmd);
    void Ph(const Cmd& cmd);
    void Amp(const Cmd& cmd);
    void Reset(const Cmd& cmd);
    void Mat(const Cmd& cmd);

private:
    Qureg m_qureg;
    MeasureResult m_result;

    std::vector<::Circuit> m_circuitCache;
    std::map<std::string, GateMatrix> m_gateMatrix;

    size_t m_runTimes;
};

END_NAMESPACE

#endif // EXECUTOR_H