#include "executor.h"
#include <iostream>
#include <string>
#include <math.h>
#include <time.h>
#include <fstream>
#include "../config/Config.h"
#include <algorithm>
#include "../../common/utility.h"
#include "../../common/Singleton.h"
#include "../common/QuESTIniter.h"
#include "../../common/qulog.h"
#include "mpi.h"

using namespace QS;
using namespace std;

using GateFunc = void (CmdExecutor::*)(const Cmd& cmd);

map<string, GateFunc> g_gateMap = {
    make_pair<string, GateFunc>("H", &CmdExecutor::H),
    make_pair<string, GateFunc>("Measure", &CmdExecutor::Measure),
    make_pair<string, GateFunc>("P", &CmdExecutor::P),
    make_pair<string, GateFunc>("CP", &CmdExecutor::CP),
    make_pair<string, GateFunc>("R", &CmdExecutor::R),
    make_pair<string, GateFunc>("Rx", &CmdExecutor::Rx), 
    make_pair<string, GateFunc>("Ry", &CmdExecutor::Ry),
    make_pair<string, GateFunc>("Rz", &CmdExecutor::Rz),
    make_pair<string, GateFunc>("Rxx", &CmdExecutor::Rxx),
    make_pair<string, GateFunc>("Ryy", &CmdExecutor::Ryy),
    make_pair<string, GateFunc>("Rzz", &CmdExecutor::Rzz),
    make_pair<string, GateFunc>("X", &CmdExecutor::X),
    make_pair<string, GateFunc>("Y", &CmdExecutor::Y),
    make_pair<string, GateFunc>("Z", &CmdExecutor::Z),
    make_pair<string, GateFunc>("S", &CmdExecutor::S),
    make_pair<string, GateFunc>("T", &CmdExecutor::T),
    make_pair<string, GateFunc>("Sdg", &CmdExecutor::Sdg),
    make_pair<string, GateFunc>("Tdg", &CmdExecutor::Tdg),
    make_pair<string, GateFunc>("SqrtX", &CmdExecutor::SqrtX),
    make_pair<string, GateFunc>("SqrtSwap", &CmdExecutor::SqrtSwap),
    make_pair<string, GateFunc>("Swap", &CmdExecutor::Swap),
    make_pair<string, GateFunc>("CNOT", &CmdExecutor::CNOT),
    make_pair<string, GateFunc>("MCX", &CmdExecutor::CNOT),
    make_pair<string, GateFunc>("CY", &CmdExecutor::CY),
    make_pair<string, GateFunc>("MCZ", &CmdExecutor::CZ),
    make_pair<string, GateFunc>("U3", &CmdExecutor::U3),
    make_pair<string, GateFunc>("U2", &CmdExecutor::U2),
    make_pair<string, GateFunc>("U1", &CmdExecutor::U1),
    make_pair<string, GateFunc>("CRx", &CmdExecutor::CRx),
    make_pair<string, GateFunc>("CRy", &CmdExecutor::CRy),
    make_pair<string, GateFunc>("CRz", &CmdExecutor::CRz),
    make_pair<string, GateFunc>("X1", &CmdExecutor::X1),
    make_pair<string, GateFunc>("Y1", &CmdExecutor::Y1),
    make_pair<string, GateFunc>("Z1", &CmdExecutor::Z1),
    make_pair<string, GateFunc>("CU1", &CmdExecutor::CU1),
    make_pair<string, GateFunc>("CU3", &CmdExecutor::CU3),
    make_pair<string, GateFunc>("U", &CmdExecutor::U),
    make_pair<string, GateFunc>("CU", &CmdExecutor::CU),
    make_pair<string, GateFunc>("CR", &CmdExecutor::CR),
    make_pair<string, GateFunc>("iSwap", &CmdExecutor::iSwap),
    make_pair<string, GateFunc>("Id", &CmdExecutor::id),
    make_pair<string, GateFunc>("CH", &CmdExecutor::CH),
    make_pair<string, GateFunc>("SqrtXdg", &CmdExecutor::SqrtXdg),
    make_pair<string, GateFunc>("CSqrtX", &CmdExecutor::CSqrtX),
    make_pair<string, GateFunc>("CSwap", &CmdExecutor::CSwap)
};

CmdExecutor::CmdExecutor()
{
    m_qureg = SINGLETON(CQuESTIniter)->m_qureg;

    m_circuitCache.clear();
    m_result.measureSet.clear();
    m_result.outcomeSet.clear();
}

CmdExecutor::~CmdExecutor() 
{
}

void CmdExecutor::receiveCircuit(const ::Circuit& circuit, const bool final) {
    // cache circuit
    m_circuitCache.push_back(circuit);

    // run circuit
    for (auto &cmd : circuit.cmds) {
        execCmd(cmd);
    }

    if (final) {
        packMeasureResult();
    }
}

double CmdExecutor::getProbOfAmp(const int64_t index) {
    return getProbAmp(m_qureg, index);
}

double CmdExecutor::getProbOfOutcome(const int32_t qubit, const int32_t outcome) {
    return calcProbOfOutcome(m_qureg, qubit, outcome);
}

void CmdExecutor::getProbOfAllOutcome(std::vector<double> & _return, const std::vector<int32_t> & qubits) {
    int numQubits = qubits.size();
    if (numQubits == 0) {
        return;
    }
    
    int numOutcomes = 1LL << numQubits;
    qreal probs[numOutcomes];   
    int bits[numQubits];
    copy(qubits.begin(), qubits.end(), bits);
    calcProbOfAllOutcomes(probs, m_qureg, bits, numQubits);
    for (auto i = 0; i < numOutcomes; ++i) {
        _return.push_back(probs[i]);
    }
}

void CmdExecutor::writeStateToFile(const std::string& id)
{
    FILE *state;
    char filename[100];
    long long int index;
    sprintf(filename, "state_rank_%s_%d.csv", id.c_str(), m_qureg.chunkId);
    state = fopen(filename, "w");
    if (m_qureg.chunkId==0) 
    {
        fprintf(state, "real, imag\n");
    }

    for(index=0; index<m_qureg.numAmpsPerChunk; index++)
    {
        # if QuEST_PREC==1 || QuEST_PREC==2
        fprintf(state, "%.12f, %.12f\n", m_qureg.stateVec.real[index], m_qureg.stateVec.imag[index]);
        # elif QuEST_PREC == 4
        fprintf(state, "%.12Lf, %.12Lf\n", m_qureg.stateVec.real[index], m_qureg.stateVec.imag[index]);
        #endif
    }
    fclose(state);
}

void CmdExecutor::getAllState(std::vector<std::string>& _return, const std::string& id) 
{
    std::stringstream ss;
    ss.precision(12);
    for(long long index = 0; index < m_qureg.numAmpsPerChunk; index++)
    {
        qreal real = m_qureg.stateVec.real[index];
        qreal imag = m_qureg.stateVec.imag[index];
        if (real > -1e-15 && real < 1e-15)
        {
            real = 0;
        }
        if (imag > -1e-15 && imag < 1e-15)
        {
            imag = 0;
        }
        ss << real << ", " << imag << '|';
    }

    int gsize = 0;
    MPI_Comm_size(MPI_COMM_WORLD, &gsize);

    const std::string& str = ss.str();
    
    std::unique_ptr<char[]> rbuf(nullptr);
    if(SINGLETON(CQuESTIniter)->m_env.rank == 0)
    {
        rbuf.reset(new(nothrow) char[gsize*(str.size())]{0});
        if (!rbuf)
        {
            LOG(ERROR) << "CmdExecutor::getAllState : not enough memory";
            return;
        }
    }
    
    MPI_Gather(str.c_str(), str.size(), MPI_CHAR, rbuf.get(), str.size(), MPI_CHAR, 0, MPI_COMM_WORLD);

    if(SINGLETON(CQuESTIniter)->m_env.rank == 0)
    {
        rbuf[gsize*(str.size())-1] = 0;

        char* substr = strtok(rbuf.get(), "|");
        while (substr != NULL)
        {
            _return.push_back(substr);
            substr = strtok(NULL, "|");
        }
    }
    // if (!m_isExistResultFile)
    // {
    //     writeStateToFile(id);
    //     m_isExistResultFile = true;
    // }

    // char filename[100];
    // sprintf(filename, "state_rank_%s_%d.csv", id.c_str(), m_qureg.chunkId);
    // fstream statefile;
	// statefile.open(filename, ifstream::in);
	// if (statefile.is_open())
	// {
	// 	string buff;
	// 	while (getline(statefile, buff))
	// 	{
	// 		_return.push_back(buff);
	// 	}
    // } 
    // statefile.close();
}

void CmdExecutor::apply_QFT(const std::vector<int32_t> & qubits) {
    int bits[qubits.size()];
    copy(qubits.begin(), qubits.end(), bits);
    applyQFT(m_qureg, bits, qubits.size());
}

void CmdExecutor::apply_Full_QFT() {
    applyFullQFT(m_qureg);
}

// 执行任务
void CmdExecutor::run(Result& result, int32_t shots) {
    // 如果只运行一次，那么在线路发送完成后已经自动运行一次了
    int run_times = shots - 1;
    while (run_times > 0) {
        m_result.measureSet.clear();
        
        // 每次运行需要重置状态
        initZeroState(m_qureg);

        for (const auto & circuit : m_circuitCache) {
            for (const auto & cmd : circuit.cmds) {
                // 执行指令
                execCmd(cmd);
            }
        }
        packMeasureResult();
        run_times -= 1;
    }

    SINGLETON(CQuESTIniter)->finalize();

    sort(m_result.outcomeSet.begin(), m_result.outcomeSet.end(), [](const Outcome& a, const Outcome& b) {return a.bitstr.compare(b.bitstr) < 0;});
    result = m_result;
    return;
}

void CmdExecutor::printCmd(const Cmd& cmd) {
    cout << cmd.desc << endl;
    LOG(INFO) << cmd.desc;
}

void CmdExecutor::packMeasureResult() {
    sort(m_result.measureSet.begin(), m_result.measureSet.end(), [](const MeasureResult& a, const MeasureResult& b) {return a.id < b.id;});
    string bitstr = "";
    for (auto &m : m_result.measureSet) {
        int val = m.value;
        bitstr.append(to_string(val));
    }

    int idx = -1;
    for (unsigned int i = 0; i < m_result.outcomeSet.size(); ++i) {
        if (m_result.outcomeSet[i].bitstr == bitstr) {
            idx = i;
            break; 
        }
    }
    if (idx >= 0) {
        m_result.outcomeSet[idx].count += 1;
    } else {
        Outcome out;
        out.bitstr = bitstr;
        out.count = 1;
        m_result.outcomeSet.push_back(out);
    }
}

void CmdExecutor::execCmd(const Cmd& cmd) {
    const auto it = g_gateMap.find(cmd.gate);
    if (it == g_gateMap.end()) {
        return;
    }

    if (SINGLETON(CConfig)->m_verbose) {
        printCmd(cmd);
    }

    (this->*(it->second))(cmd);
}

bool CmdExecutor::isInit()
{
    return SINGLETON(CQuESTIniter)->m_isInit;
}

void CmdExecutor::Measure(const Cmd& cmd) {
    for (auto id : cmd.targets) {
        int ret = measure(m_qureg, id);
        MeasureResult mr;
        mr.id = id;
        mr.value = ret;
        this->m_result.measureSet.push_back(mr);
    }
}

double CmdExecutor::getExpecPauliProd(const std::vector<PauliProdInfo>& pauli_prod) {
    size_t prodsize = pauli_prod.size();
    if (0 == prodsize) {
        LOG(ERROR) << "getExpecPauliProd Invaild parameters prodsize is empty";
        return 0;
    }

    Qureg workqureg;
    SINGLETON(CQuESTIniter)->createWorkQureg(workqureg);

    std::unique_ptr<int []> targets(new int[prodsize]());
    std::unique_ptr<pauliOpType []> paulitype(new pauliOpType[prodsize]());
    for (size_t i = 0; i < prodsize; ++i) {
        targets[i] = pauli_prod[i].target;
        paulitype[i] = (pauliOpType)pauli_prod[i].oper_type;
    }
    double expectvalue = calcExpecPauliProd(m_qureg, targets.get(), paulitype.get(), prodsize, workqureg);

    SINGLETON(CQuESTIniter)->destroyWorkQureg(workqureg);
    return expectvalue;

}

double CmdExecutor::getExpecPauliSum(const std::vector<PauliOperType::type>& oper_type_list, const std::vector<double>& term_coeff_list) {
    size_t typesize = oper_type_list.size();
    size_t coeffsize = term_coeff_list.size();
    if (coeffsize*SINGLETON(CQuESTIniter)->m_qubits != typesize) {
        LOG(ERROR) << "getExpecPauliSum Invaild parameters(coeffsize:" << coeffsize << ",typesize:" << typesize << ").";
        return 0;
    }
    
    Qureg workqureg;
    SINGLETON(CQuESTIniter)->createWorkQureg(workqureg);

    std::unique_ptr<pauliOpType []> paulitype(new pauliOpType[typesize]());
    std::unique_ptr<qreal []> coeffs(new qreal[coeffsize]());
    for (size_t i = 0; i < typesize; ++i) {
        paulitype[i] = (pauliOpType)oper_type_list[i];
    }
    for (size_t i = 0; i < coeffsize; ++i) {
        coeffs[i] = term_coeff_list[i];
    }

    double expectvalue = calcExpecPauliSum(m_qureg, paulitype.get(), coeffs.get(), coeffsize, workqureg);

    SINGLETON(CQuESTIniter)->destroyWorkQureg(workqureg);
    return expectvalue;
}

void CmdExecutor::H(const Cmd& cmd) {
    if (cmd.targets.size() != 1) {
        return;
    }
    int targetQubit = cmd.targets[0];

    if (cmd.inverse) {
       //逆操作和正常操作一样
    }

    hadamard(m_qureg, targetQubit);
}

void CmdExecutor::CH(const Cmd& cmd) {
    if (cmd.targets.size() != 1 || cmd.controls.size() != 1) {
        return;
    } 

    if (cmd.inverse) {
        //逆操作和正常操作一样
    }

    double factor = 1 / sqrt(2);
    ComplexMatrix2 m = {
            {
                {1 * factor, 1 * factor}, 
                {1 * factor, -1 * factor}
            },
            {
                {0, 0}, 
                {0, 0}
            }
    };

    controlledUnitary(m_qureg, cmd.controls[0], cmd.targets[0], m);
}

void CmdExecutor::barrier(const Cmd& cmd) {
    // barrier在projectq起到分隔命令阻止命令优化的作用，并没有实际含�
}

void CmdExecutor::P(const Cmd& cmd) {
    // matrix([[cmath.exp(1j * self.angle), 0], [0, cmath.exp(1j * self.angle)]]
    // if (cmd.targets.size() != 1) {
    //     return;
    // }

    // double angle = cmd.rotation[0];
    // ComplexMatrix2 m = {
    //     .real = {{cos(angle), 0}, 
    //             {0, cos(angle)}},
    //     .imag = {{sin(angle), 0}, 
    //             {0, sin(angle)}}};
    // applyMatrix2(m_qureg, cmd.targets[0], m);

    if (cmd.targets.size() != 1 || cmd.rotation.size() != 1) {
        return;
    }

    double rotation = cmd.rotation[0];
    if (cmd.inverse) {
        rotation = -rotation;
    }

    int targetQubit = cmd.targets[0];
    phaseShift(m_qureg, targetQubit, rotation); 
}

void CmdExecutor::CP(const Cmd& cmd) {
    if (cmd.rotation.size() != 1 || cmd.targets.size() != 1 || cmd.controls.size() != 1) {
        return;
    }

    double rotation = cmd.rotation[0];
    if (cmd.inverse) {
        rotation = -rotation;
    }

    controlledPhaseShift(m_qureg, cmd.controls[0], cmd.targets[0], rotation);
}

void CmdExecutor::R(const Cmd& cmd) {
    if (cmd.targets.size() != 1 || cmd.rotation.size() != 2) {
        return;
    }

    // phaseShift(g_qureg, targets[0], rotation[0]);
    // matrix
    // theta, phi = float(self.alpha), float(self.beta)
    // cos = math.cos(theta / 2)
    // sin = math.sin(theta / 2)
    // exp_m = numpy.exp(-1j * phi) = cos(phi)-sin(phi)
    // exp_p = numpy.exp(1j * phi) = cos(phi)+sin(phi)
    // numpy.array([[cos, -1j * exp_m * sin], [-1j * exp_p * sin, cos]])

    auto theta = cmd.rotation[0];
    if (cmd.inverse) {
        theta = -theta;
    }
    auto phi = cmd.rotation[1];

    ComplexMatrix2 m = {
        .real = {
            {cos(theta/2), sin(-phi) * sin(theta/2)}, 
            {sin(phi) * sin(theta/2), cos(theta/2)}
        },
        .imag = {
            {0, -1 * cos(-phi) * sin(theta/2)}, 
            {-1 * cos(phi) * sin(theta/2), 0}
        }
    };

    applyMatrix2(m_qureg, cmd.targets[0], m); 
}

void CmdExecutor::X(const Cmd& cmd) {
    if (cmd.targets.size() != 1) {
        return;
    }
    int targetQubit = cmd.targets[0];

    if (cmd.inverse) {
        //逆操作和正常操作一样
    }

    pauliX(m_qureg, targetQubit);
}

void CmdExecutor::Y(const Cmd& cmd) {
    if (cmd.targets.size() != 1) {
        return;
    }
    int targetQubit = cmd.targets[0];

    if (cmd.inverse) {
        //逆操作和正常操作一样
    }

    pauliY(m_qureg, targetQubit);
}

void CmdExecutor::Z(const Cmd& cmd) {
    if (cmd.targets.size() != 1) {
        return;
    }
    int targetQubit = cmd.targets[0];

    if (cmd.inverse) {
        //逆操作和正常操作一样
    }

    pauliZ(m_qureg, targetQubit);
}

void CmdExecutor::Rx(const Cmd& cmd) {
    if (cmd.targets.size() != 1) {
        return;
    }

    double rotation = cmd.rotation[0];
    if (cmd.inverse) {
        rotation = -rotation;
    }

    int targetQubit = cmd.targets[0];
    rotateX(m_qureg, targetQubit, rotation);
}

void CmdExecutor::Ry(const Cmd& cmd) {
    if (cmd.targets.size() != 1) {
        return;
    }

    double rotation = cmd.rotation[0];
    if (cmd.inverse) {
        rotation = -rotation;
    }

    int targetQubit = cmd.targets[0];
    rotateY(m_qureg, targetQubit, rotation);
}

void CmdExecutor::Rz(const Cmd& cmd) {
    if (cmd.targets.size() != 1) {
        return;
    }

    double rotation = cmd.rotation[0];
    if (cmd.inverse) {
        rotation = -rotation;
    }

    int targetQubit = cmd.targets[0];
    rotateZ(m_qureg, targetQubit, rotation);
}

void CmdExecutor::S(const Cmd& cmd) {
    if (cmd.targets.size() != 1) {
        return;
    }
    
    if (cmd.inverse) {
        Cmd temp(cmd);
        temp.inverse = false;
        Sdg(temp);
    } else {
        int targetQubit = cmd.targets[0];
        sGate(m_qureg, targetQubit);
    }
}

void CmdExecutor::T(const Cmd& cmd) {
    if (cmd.inverse) {
        Cmd temp(cmd);
        temp.inverse = false;
        Tdg(temp);
    } else {
        int targetQubit = cmd.targets[0];
        tGate(m_qureg, targetQubit);
    }
}

void CmdExecutor::Sdg(const Cmd& cmd) {
    if (cmd.inverse) {
        Cmd temp(cmd);
        temp.inverse = false;
        S(temp);
    } else {
        // sgate matrix([[1, 0], [0, 1j]])
        // 共轭转置：matrix([[1, 0], [0, -1j]])
        ComplexMatrix2 m = {
            .real = {
                {1, 0},
                {0, 0}
            },
            .imag = {
                {0, 0},
                {0, -1}
            }
        };
        applyMatrix2(m_qureg, cmd.targets[0], m); 
    }
}

void CmdExecutor::Tdg(const Cmd& cmd) {
    if (cmd.inverse) {
        Cmd temp(cmd);
        temp.inverse = false;
        T(temp);
    } else {
        // matrix([[1, 0], [0, cmath.exp(1j * cmath.pi / 4)]])
        // 共轭转置：matrix([[1, 0], [0, cmath.exp(-1j * cmath.pi / 4)]])
        ComplexMatrix2 m = {
            .real = {
                {1, 0},
                {0, 1/sqrt(2)}
            },
            .imag = {
                {0, 0},
                {0, -1/sqrt(2)}
            }
        };
        applyMatrix2(m_qureg, cmd.targets[0], m); 
    }
}

void CmdExecutor::SqrtX(const Cmd& cmd) {
    if (cmd.targets.size() != 1) {
        return;
    }

    if (cmd.inverse) {
        ComplexMatrix2 mdg = {
            .real = {
                {0.5, 0.5},
                {0.5, 0.5}
            },
            .imag = {
                {-0.5, 0.5},
                {0.5, -0.5}
            }
        };
        applyMatrix2(m_qureg, cmd.targets[0], mdg);
    } else {
        // projectq matrix
        // 0.5 * np.matrix([[1 + 1j, 1 - 1j], [1 - 1j, 1 + 1j]])
        ComplexMatrix2 m = {
            .real = {
                {0.5, 0.5},
                {0.5, 0.5}
            },
            .imag = {
                {0.5, -0.5},
                {-0.5, 0.5}
            }
        };
        applyMatrix2(m_qureg, cmd.targets[0], m);
    }
}

void CmdExecutor::SqrtXdg(const Cmd& cmd)
{
    if (cmd.targets.size() != 1) {
        return;
    }
    
    if (cmd.inverse) {
        // projectq matrix
        // 0.5 * np.matrix([[1 + 1j, 1 - 1j], [1 - 1j, 1 + 1j]])
        ComplexMatrix2 mdg = {
            .real = {
                {0.5, 0.5},
                {0.5, 0.5}
            },
            .imag = {
                {0.5, -0.5},
                {-0.5, 0.5}
            }
        };
        applyMatrix2(m_qureg, cmd.targets[0], mdg);
    } else {
        ComplexMatrix2 m = {
            .real = {
                {0.5, 0.5},
                {0.5, 0.5}
            },
            .imag = {
                {-0.5, 0.5},
                {0.5, -0.5}
            }
        };
        applyMatrix2(m_qureg, cmd.targets[0], m);
    }
}

void CmdExecutor::CSqrtX(const Cmd& cmd) {
    if (cmd.targets.size() != 1 || cmd.controls.size() != 1) {
        return;
    }

    if (cmd.inverse) {
        ComplexMatrix2 mdg = {
            .real = {
                {0.5, 0.5},
                {0.5, 0.5}
            },
            .imag = {
                {-0.5, 0.5},
                {0.5, -0.5}
            }
        };
        controlledUnitary(m_qureg, cmd.controls[0], cmd.targets[0], mdg);
    } else {
        // projectq matrix
        // 0.5 * np.matrix([[1 + 1j, 1 - 1j], [1 - 1j, 1 + 1j]])
        ComplexMatrix2 m = {
            .real = {
                {0.5, 0.5},
                {0.5, 0.5}
            },
            .imag = {
                {0.5, -0.5},
                {-0.5, 0.5}
            }
        };
        controlledUnitary(m_qureg, cmd.controls[0], cmd.targets[0], m);
    }
}

void CmdExecutor::SqrtSwap(const Cmd& cmd) {
    if (cmd.targets.size() != 2) {
        return;
    }

    if (cmd.inverse) {
        // matrix(
        //     [
        //         [1, 0, 0, 0],
        //         [0, 0.5 - 0.5j, 0.5 + 0.5j, 0],
        //         [0, 0.5 + 0.5j, 0.5 - 0.5j, 0],
        //         [0, 0, 0, 1],
        //     ]
        // )
        ComplexMatrix4 m = {
            .real = {{1, 0, 0, 0},
                {0, 0.5, 0.5, 0},
                {0, 0.5, 0.5, 0},
                {0, 0, 0, 1}
            },
            .imag = {
                {0, 0, 0, 0},
                {0, -0.5, 0.5, 0},
                {0, 0.5, -0.5, 0},
                {0, 0, 0, 0}
            }
        };

        applyMatrix4(m_qureg, cmd.targets[0], cmd.targets[1], m);
    } else {
        // matrix(
        //     [
        //         [1, 0, 0, 0],
        //         [0, 0.5 + 0.5j, 0.5 - 0.5j, 0],
        //         [0, 0.5 - 0.5j, 0.5 + 0.5j, 0],
        //         [0, 0, 0, 1],
        //     ]
        // )
        ComplexMatrix4 m = {
            .real = {{1, 0, 0, 0},
                {0, 0.5, 0.5, 0},
                {0, 0.5, 0.5, 0},
                {0, 0, 0, 1}
            },
            .imag = {
                {0, 0, 0, 0},
                {0, 0.5, -0.5, 0},
                {0, -0.5, 0.5, 0},
                {0, 0, 0, 0}
            }
        };

        applyMatrix4(m_qureg, cmd.targets[0], cmd.targets[1], m);
    }
}

void CmdExecutor::Rxx(const Cmd& cmd) {
    // projectq matrix
    // matrix(
    //         [
    //             [cmath.cos(0.5 * self.angle), 0, 0, -1j * cmath.sin(0.5 * self.angle)],
    //             [0, cmath.cos(0.5 * self.angle), -1j * cmath.sin(0.5 * self.angle), 0],
    //             [0, -1j * cmath.sin(0.5 * self.angle), cmath.cos(0.5 * self.angle), 0],
    //             [-1j * cmath.sin(0.5 * self.angle), 0, 0, cmath.cos(0.5 * self.angle)],
    //         ]
    //     )
    if (cmd.targets.size() != 2) {
        return;
    }

    double angle = cmd.rotation[0];
    if (cmd.inverse) {
        angle = -angle;
    }

    ComplexMatrix4 m = {
        .real = {{cos(0.5 * angle), 0, 0, 0},
            {0, cos(0.5 * angle), 0, 0},
            {0, 0, cos(0.5 * angle), 0},
            {0, 0, 0, cos(0.5 * angle)}
        },
        .imag = {
            {0, 0, 0, -1 * sin(0.5 * angle)},
            {0, 0, -1 * sin(0.5 * angle), 0},
            {0, -1 * sin(0.5 * angle), 0, 0},
            {-1 * sin(0.5 * angle), 0, 0, 0}
        }
    };

    applyMatrix4(m_qureg, cmd.targets[0], cmd.targets[1], m);
}

void CmdExecutor::Ryy(const Cmd& cmd) {
    // projectq matrix
    // matrix(
    //     [
    //         [cmath.cos(0.5 * self.angle), 0, 0, 1j * cmath.sin(0.5 * self.angle)],
    //         [0, cmath.cos(0.5 * self.angle), -1j * cmath.sin(0.5 * self.angle), 0],
    //         [0, -1j * cmath.sin(0.5 * self.angle), cmath.cos(0.5 * self.angle), 0],
    //         [1j * cmath.sin(0.5 * self.angle), 0, 0, cmath.cos(0.5 * self.angle)],
    //     ]
    // )

    if (cmd.targets.size() != 2) {
        return;
    }

    double angle = cmd.rotation[0];
    if (cmd.inverse) {
        angle = -angle;
    }
    ComplexMatrix4 m = {
        .real = {{cos(0.5 * angle), 0, 0, 0},
            {0, cos(0.5 * angle), 0, 0},
            {0, 0, cos(0.5 * angle), 0},
            {0, 0, 0, cos(0.5 * angle)}
        },
        .imag = {
            {0, 0, 0, 1 * sin(0.5 * angle)},
            {0, 0, -1 * sin(0.5 * angle), 0},
            {0, -1 * sin(0.5 * angle), 0, 0},
            {1 * sin(0.5 * angle), 0, 0, 0}
        }
    };
    applyMatrix4(m_qureg, cmd.targets[0], cmd.targets[1], m);
}

void CmdExecutor::Rzz(const Cmd& cmd) {
    // projectq matrix
    // matrix(
    //         [
    //             [cmath.exp(-0.5 * 1j * self.angle), 0, 0, 0],
    //             [0, cmath.exp(0.5 * 1j * self.angle), 0, 0],
    //             [0, 0, cmath.exp(0.5 * 1j * self.angle), 0],
    //             [0, 0, 0, cmath.exp(-0.5 * 1j * self.angle)],
    //         ]
    //     )

    if (cmd.rotation.size() != 1 || cmd.targets.size() != 2) {
        return;
    }

    double angle = cmd.rotation[0];
    if (cmd.inverse) {
        angle = -angle;
    }
    ComplexMatrix4 m = {
        .real = {
            {cos(0.5 * angle), 0, 0, 0},
            {0, cos(0.5 * angle), 0, 0},
            {0, 0, cos(0.5 * angle), 0},
            {0, 0, 0, cos(0.5 * angle)}
        },
        .imag = {
            {-1 * sin(0.5 * angle), 0, 0, 0},
            {0, sin(0.5 * angle), 0, 0},
            {0, 0, sin(0.5 * angle), 0},
            {0, 0, 0, -1 * sin(0.5 * angle)}
        }
    };
    applyMatrix4(m_qureg, cmd.targets[0], cmd.targets[1], m);
}

void CmdExecutor::Swap(const Cmd& cmd) {
    if (cmd.targets.size() != 2) {
        return;
    }

    if (cmd.inverse) {
        //逆操作和正常操作一样
    }

    swapGate(m_qureg, cmd.targets[0], cmd.targets[1]);
}

void CmdExecutor::CSwap(const Cmd& cmd) {
    if (cmd.targets.size() != 2 || cmd.controls.size() != 1) {
        return;
    } 

    if (cmd.inverse) {
        //逆操作和正常操作一样
    }

    ComplexMatrix4 m = {
            {
                {1, 0, 0, 0}, 
                {0, 0, 1, 0},
                {0, 1, 0, 0},
                {0, 0, 0, 1}
            },
            {
                {0, 0, 0, 0},
                {0, 0, 0, 0},
                {0, 0, 0, 0},
                {0, 0, 0, 0}
            }
    };

    controlledTwoQubitUnitary(m_qureg, cmd.controls[0], cmd.targets[0], cmd.targets[1], m);
}

void CmdExecutor::CNOT(const Cmd& cmd) {
    if (cmd.targets.size() == 1 && cmd.controls.size() == 1) {
        if (cmd.inverse) {
            //逆操作和正常操作一样
        }
        controlledNot(m_qureg, cmd.controls[0], cmd.targets[0]);
    } else {
        int ctrls[cmd.controls.size()];
        copy(cmd.controls.begin(), cmd.controls.end(), ctrls);
        int targs[cmd.targets.size()];
        copy(cmd.targets.begin(), cmd.targets.end(), targs);
        if (cmd.inverse) {
            //逆操作和正常操作一样
        }
        multiControlledMultiQubitNot(m_qureg, 
            ctrls, 
            cmd.controls.size(), 
            targs, 
            cmd.targets.size()
        ); 
    }
}

void CmdExecutor::CY(const Cmd& cmd) {
    if (cmd.targets.size() != 1 || cmd.controls.size() != 1) {
        return;
    } 

    if (cmd.inverse) {
        //逆操作和正常操作一样
    }

    controlledPauliY(m_qureg, cmd.controls[0], cmd.targets[0]);
}

void CmdExecutor::CZ(const Cmd& cmd) {
    // cmd.controls.insert(cmd.controls.cend(), cmd.targets.cbegin(), cmd.targets.cend());
    vector<int32_t> tempCtls;
    tempCtls.assign(cmd.controls.begin(), cmd.controls.end());
    tempCtls.insert(tempCtls.end(), cmd.targets.begin(), cmd.targets.end());
    int ctrls[tempCtls.size()];
    copy(tempCtls.begin(), tempCtls.end(), ctrls);
    
    if (cmd.inverse) {
        //逆操作和正常操作一样
    }

    multiControlledPhaseFlip(m_qureg, 
        ctrls, 
        tempCtls.size()
    );
}

void CmdExecutor::U3(const Cmd& cmd) {
    if (cmd.rotation.size() != 3 || cmd.targets.size() != 1) {
        return;
    }

    auto theta = cmd.rotation[0];
    auto phi = cmd.rotation[1];
    auto lam = cmd.rotation[2];
    if (cmd.inverse) {
        auto theta1 = -theta;
        auto phi1 = -lam;
        auto lam1 = -phi;
        theta = theta1;
        phi = phi1;
        lam = lam1;
    }
    
    ComplexMatrix2 m = {
        .real = {
            {cos(theta / 2), -1 * cos(lam) * sin(theta / 2) },
            {cos(phi) * sin(theta / 2) , cos(phi + lam) * cos(theta / 2)}
        },
        .imag = {
            {0, -1 * sin(lam) * sin(theta / 2)},
            {sin(phi) * sin(theta / 2), sin(phi + lam) * cos(theta / 2)}
        }
    };
    unitary(m_qureg, cmd.targets[0], m);
}

void CmdExecutor::U2(const Cmd& cmd) {
    if (cmd.rotation.size() != 2 || cmd.targets.size() != 1) {
        return;
    }

    auto phi = cmd.rotation[0];
    auto lam = cmd.rotation[1];

    if (cmd.inverse) {
        auto phi1 = -lam - M_PI;
        auto lam1 = -phi + M_PI;
        phi = phi1;
        lam = lam1;
    }

    double factor = 1 / sqrt(2);
    ComplexMatrix2 m = {
        .real = {
            {1 * factor, -factor * cos(lam)},
            {factor * cos(phi), factor * cos(phi + lam)}
        },
        .imag = {
            {0, -factor * sin(lam)},
            {factor * sin(phi), factor * sin(phi + lam)}
        }
    };
    unitary(m_qureg, cmd.targets[0], m);
}

void CmdExecutor::U1(const Cmd& cmd) {
    if (cmd.rotation.size() != 1 || cmd.targets.size() != 1) {
        return;
    }

    auto alpha = cmd.rotation[0];

    if (cmd.inverse) {
        alpha = -alpha;
    }

    ComplexMatrix2 m = {
        .real = {
            {1, 0},
            {0, cos(alpha)}
        },
        .imag = {
            {0, 0},
            {0, sin(alpha)}
        }
    };
    unitary(m_qureg, cmd.targets[0], m);
}

void CmdExecutor::CRx(const Cmd& cmd) {
    if (cmd.rotation.size() != 1 || cmd.targets.size() != 1 || cmd.controls.size() != 1) {
        return;
    }

    double rotation = cmd.rotation[0];
    if (cmd.inverse) {
        rotation = -rotation;
    }

    controlledRotateX(m_qureg, cmd.controls[0], cmd.targets[0], rotation);
}

void CmdExecutor::CRy(const Cmd& cmd) {
    if (cmd.rotation.size() != 1 || cmd.targets.size() != 1 || cmd.controls.size() != 1) {
        return;
    }

    double rotation = cmd.rotation[0];
    if (cmd.inverse) {
        rotation = -rotation;
    }

    controlledRotateY(m_qureg, cmd.controls[0], cmd.targets[0], rotation);
}

void CmdExecutor::CRz(const Cmd& cmd) {
    if (cmd.rotation.size() != 1 || cmd.targets.size() != 1 || cmd.controls.size() != 1) {
        return;
    }

    double rotation = cmd.rotation[0];
    if (cmd.inverse) {
        rotation = -rotation;
    }

    controlledRotateZ(m_qureg, cmd.controls[0], cmd.targets[0], rotation);
}

void CmdExecutor::X1(const Cmd& cmd)
{
    if (cmd.targets.size() != 1) {
        return;
    }

    if (cmd.inverse) {
        double factor = sqrt(2);
        ComplexMatrix2 m = {
            .real = {
                {0.5 * factor, 0}, 
                {0, 0.5 * factor}
            },
            .imag = {
                {0, 0.5 * factor}, 
                {0.5 * factor, 0}
            }
        };

        applyMatrix2(m_qureg, cmd.targets[0], m); 
    } else {
        double factor = 1 / sqrt(2);
        ComplexMatrix2 m = {
            .real = {
                {1 * factor, 0}, 
                {0, 1 * factor}
            },
            .imag = {
                {0, -1 * factor}, 
                {-1 * factor, 0}
            }
        };

        applyMatrix2(m_qureg, cmd.targets[0], m); 
    }
}

void CmdExecutor::Y1(const Cmd& cmd)
{
    if (cmd.targets.size() != 1) {
        return;
    }

    if (cmd.inverse) {
        double factor = sqrt(2);
        ComplexMatrix2 m = {
            .real = {
                {0.5 * factor, 0.5 * factor}, 
                {-0.5 * factor, 0.5 * factor}
            },
            .imag = {
                {0, 0}, 
                {0, 0}
            }
        };

        applyMatrix2(m_qureg, cmd.targets[0], m); 
    } else {
        double factor = 1 / sqrt(2);
        ComplexMatrix2 m = {
            .real = {
                {1 * factor, -1 * factor}, 
                {1 * factor, 1 * factor}
            },
            .imag = {
                {0, 0}, 
                {0, 0}
            }
        };

        applyMatrix2(m_qureg, cmd.targets[0], m); 
    }
}

void CmdExecutor::Z1(const Cmd& cmd)
{
    if (cmd.targets.size() != 1) {
        return;
    }

    double rotation = M_PI/4.0;
    if (cmd.inverse) {
        rotation = -rotation;
    } 

    ComplexMatrix2 m = {
        .real = {
            {cos(-rotation), 0}, 
            {0, cos(rotation)}
        },
        .imag = {
            {sin(-rotation), 0}, 
            {0, sin(rotation)}
        }
    };

    applyMatrix2(m_qureg, cmd.targets[0], m); 
}

void CmdExecutor::CU1(const Cmd& cmd)
{
    if (cmd.rotation.size() != 1 || cmd.targets.size() != 1 || cmd.controls.size() !=1) {
        return;
    }

    auto alpha = cmd.rotation[0];

    if (cmd.inverse) {
        alpha = -alpha;
    }

    ComplexMatrix4 m = {
        .real = {
            {1, 0, 0, 0},
            {0, 1, 0, 0},
            {0, 0, 1, 0},
            {0, 0, 0, cos(alpha)}
        },
        .imag = {
            {0, 0, 0, 0},
            {0, 0, 0, 0},
            {0, 0, 0, 0},
            {0, 0, 0, sin(alpha)}
        }
    };
    applyMatrix4(m_qureg, cmd.controls[0], cmd.targets[0], m);
}

void CmdExecutor::CU3(const Cmd& cmd)
{
    if (cmd.rotation.size() != 3 || cmd.targets.size() != 1 || cmd.controls.size() != 1) {
        return;
    }

    auto theta = cmd.rotation[0];
    auto phi = cmd.rotation[1];
    auto lam = cmd.rotation[2];
    if (cmd.inverse) {
        auto theta1 = -theta;
        auto phi1 = -lam;
        auto lam1 = -phi;
        theta = theta1;
        phi = phi1;
        lam = lam1;
    }

    ComplexMatrix4 m = {
        .real = {
            {1, 0, 0, 0},
            {0, cos(theta/2), 0, -cos(lam)*sin(theta/2)},
            {0, 0, 1, 0},
            {0, cos(phi)*sin(theta/2), 0, cos(phi + lam)*cos(theta/2)}
        },
        .imag = {
            {0, 0, 0, 0},
            {0, 0, 0, -sin(lam)*sin(theta/2)},
            {0, 0, 0, 0},
            {0, sin(phi)*sin(theta/2), 0, sin(phi + lam)*cos(theta/2)}
        }
    };
    applyMatrix4(m_qureg, cmd.controls[0], cmd.targets[0], m);
}

void CmdExecutor::U(const Cmd& cmd)
{
    U3(cmd);
}

void CmdExecutor::CU(const Cmd& cmd)
{
    if (cmd.rotation.size() != 4 || cmd.targets.size() != 1 || cmd.controls.size() != 1) {
        return;
    }

    auto theta = cmd.rotation[0];
    auto phi = cmd.rotation[1];
    auto lam = cmd.rotation[2];
    auto gamma = cmd.rotation[3];
    if (cmd.inverse) {
        auto theta1 = -theta;
        auto phi1 = -lam;
        auto lam1 = -phi;
        auto gamma1 = -gamma;
        theta = theta1;
        phi = phi1;
        lam = lam1;
        gamma = gamma1;
    }

    ComplexMatrix4 m = {
        .real = {
            {1, 0, 0, 0},
            {0, cos(gamma)*cos(theta/2), 0, -cos(gamma + lam)*sin(theta/2)},
            {0, 0, 1, 0},
            {0, cos(gamma + phi)*sin(theta/2), 0, cos(gamma + phi + lam)*cos(theta/2)}
        },
        .imag = {
            {0, 0, 0, 0},
            {0, sin(gamma)*cos(theta/2), 0, -sin(gamma + lam)*sin(theta/2)},
            {0, 0, 0, 0},
            {0, sin(gamma + phi)*sin(theta/2), 0, sin(gamma + phi + lam)*cos(theta/2)}
        }
    };
    applyMatrix4(m_qureg, cmd.controls[0], cmd.targets[0], m);
}

void CmdExecutor::CR(const Cmd& cmd)
{
    if (cmd.rotation.size() != 1 || cmd.targets.size() != 1 || cmd.controls.size() != 1) {
        return;
    }

    auto theta = cmd.rotation[0];
    if (cmd.inverse) {
        theta = -theta;
    }

    ComplexMatrix4 m = {
        .real = {
            {1, 0, 0, 0},
            {0, 1, 0, 0},
            {0, 0, 1, 0},
            {0, 0, 0, cos(theta)}
        },
        .imag = {
            {0, 0, 0, 0},
            {0, 0, 0, 0},
            {0, 0, 0, 0},
            {0, 0, 0, sin(theta)}
        }
    };
    applyMatrix4(m_qureg, cmd.controls[0], cmd.targets[0], m);
}

void CmdExecutor::iSwap(const Cmd& cmd)
{
    if (cmd.rotation.size() != 1 || cmd.targets.size() != 2) {
        return;
    }

    auto theta = cmd.rotation[0];
    if (cmd.inverse) {
        theta = -theta;
    }

    ComplexMatrix4 m = {
        .real = {
            {1, 0, 0, 0},
            {0, cos(theta), 0, 0},
            {0, 0, cos(theta), 0},
            {0, 0, 0, 1}
        },
        .imag = {
            {0, 0, 0, 0},
            {0, 0, -sin(theta), 0},
            {0, -sin(theta), 0, 0},
            {0, 0, 0, 0}
        }
    };
    applyMatrix4(m_qureg, cmd.targets[0], cmd.targets[1], m);
}

void CmdExecutor::id(const Cmd& cmd)
{
    // do nothing
}