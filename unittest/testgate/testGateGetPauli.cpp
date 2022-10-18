#include <sstream>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include "testGateGetPauli.h"
#include "common/Singleton.h"
#include "common/QuESTIniter.h"
#include "command/executor.h"

using namespace QS;

void default_invalidQuESTInputError(const char* errMsg, const char* errFunc) {
    printf("!!!\n");
    printf("QuEST Error in function %s: %s\n", errFunc, errMsg);
    printf("!!!\n");
    printf("exiting..\n");
    exit(EXIT_FAILURE);
}

void QuESTAssert(int isValid, int code, const char* func){
    if (!isValid) default_invalidQuESTInputError("11111111111111111111111", func);
}

int isValidPauliCode(enum pauliOpType code) {
    std::cout << "-------------isValidPauliCode--------(code:" << code << ")" << std::endl;
    return (code==PAULI_I || code==PAULI_X || code==PAULI_Y || code==PAULI_Z);
}

void validatePauliCodes(enum pauliOpType* pauliCodes, int numPauliCodes, const char* caller) {
    for (int i=0; i < numPauliCodes; i++) {
        enum pauliOpType code = pauliCodes[i];
        QuESTAssert(isValidPauliCode(code), -1, caller);
    }
}

qreal calcExpecPauliSumtEx(Qureg qureg, enum pauliOpType* allPauliCodes, qreal* termCoeffs, int numSumTerms, Qureg workspace) {
    validatePauliCodes(allPauliCodes, numSumTerms*qureg.numQubitsRepresented, __func__);
    
    return 0;
}

double getExpecPauliSum(const std::vector<PauliOperType::type>& oper_type_list, const std::vector<double>& term_coeff_list) {
    size_t opersize = oper_type_list.size();
    if (0 == opersize) {
        return 0;
    }

    size_t coeffsize = term_coeff_list.size();
    if (0 == coeffsize) {
        return 0;
    }

    Qureg workqureg;
    SINGLETON(CQuESTIniter)->createWorkQureg(workqureg);

    std::unique_ptr<pauliOpType []> paulitype(new pauliOpType[opersize]());
    std::unique_ptr<qreal []> coeffs(new qreal[coeffsize]());
    for (size_t i = 0; i < opersize; ++i) {
        paulitype[i] = (pauliOpType)oper_type_list[i];
        std::cout << "-------------getExpecPauliSum--------(paulitype:" << oper_type_list[i] << ")" << std::endl;
    }
    for (size_t i = 0; i < coeffsize; ++i) {
        std::cout << "-------------getExpecPauliSum--------(coeffs:" << term_coeff_list[i] << ")" << std::endl;
        coeffs[i] = term_coeff_list[i];
    }

    double expectvalue = calcExpecPauliSumtEx(SINGLETON(CQuESTIniter)->m_qureg, paulitype.get(), coeffs.get(), opersize, workqureg);

    SINGLETON(CQuESTIniter)->destroyWorkQureg(workqureg);
    return expectvalue;
}

CTestGateGetPauli::CTestGateGetPauli()
{

}

CTestGateGetPauli::~CTestGateGetPauli()
{

}

bool CTestGateGetPauli::TestInv()
{
    std::ostringstream os("");
    std::ostringstream osprint("");

    int qubits = 2;
    bool density = false;
    SINGLETON(CQuESTIniter)->Init(qubits, density);

    CmdExecutor handle;

    for (auto i = 0; i < qubits; ++i)
    {
        os.str("");
        Cmd cmd;
        cmd.__set_gate("H");
        cmd.targets.push_back(i);
        os << "h q[" << i << "]";
        cmd.desc = os.str();
        cmd.inverse = false;
        handle.H(cmd);
    }

    std::vector<std::string> initmatrix;
    handle.getAllState(initmatrix);
    osprint << "test GetPauli inverse 初始振幅:[";
    for (auto temp : initmatrix)
    {
        osprint << "[" << temp << "],";
    }
    osprint << "] ";

    {
        time_t now = time(NULL);
        std::vector<PauliOperType::type> oper_type_list;
        std::vector<double> term_coeff_list;
        for (auto i = 0; i < 4*qubits; ++i)
        {
            srand(now + i);
            oper_type_list.push_back((PauliOperType::type)(rand() % 4));

            double coeff = rand() % 1000;
            coeff += coeff/100.0;
            if (rand() % 2 == 0)
            {
                coeff = -coeff;
            }
            term_coeff_list.push_back(coeff);
        }
        double e = handle.getExpecPauliSum(oper_type_list, term_coeff_list);

        os.str("");
        os << "getExpecPauliSum:" << e;
    }
    std::cout << osprint.str() << std::endl;
    std::cout << os.str() << std::endl;

    return true;
}

TEST_F(CTestGateGetPauli, TestInv)
{
	EXPECT_TRUE(TestInv());
}
