#include <sstream>
#include <iostream>
#include <fstream>
#include "testGateCNOT.h"
#include "common/Singleton.h"
#include "common/QuESTIniter.h"
#include "command/executor.h"

using namespace QS;

CTestGateCNOT::CTestGateCNOT()
{

}

CTestGateCNOT::~CTestGateCNOT()
{

}

/*
测试结果：
Cmd(gate=CX, targets=[1], controls=[0], rotation=[], desc=cx q[0] q[1], inverse=0)
test CNOT inverse state:Zero 初始振幅:[[0.707106781187, 0],[0.707106781187, 0],[0, 0],[0, 0],] 应用门后的振幅:[[0.707106781187, 0],[0, 0],[0, 0],[0.707106781187, 0],] 逆操作后的振幅:[[0.707106781187, 0],[0.707106781187, 0],[0, 0],[0, 0],] 
Cmd(gate=CX, targets=[1], controls=[0], rotation=[], desc=cx q[0] q[1], inverse=0)
test CNOT inverse state:Plus 初始振幅:[[0.707106781187, 0],[0, 0],[0.707106781187, 0],[0, 0],] 应用门后的振幅:[[0.707106781187, 0],[0, 0],[0.707106781187, 0],[0, 0],] 逆操作后的振幅:[[0.707106781187, 0],[0, 0],[0.707106781187, 0],[0, 0],] 
Cmd(gate=CX, targets=[1], controls=[0], rotation=[], desc=cx q[0] q[1], inverse=0)
test CNOT inverse state:Classical 初始振幅:[[0.707106781187, 0],[0.707106781187, 0],[0, 0],[0, 0],] 应用门后的振幅:[[0.707106781187, 0],[0, 0],[0, 0],[0.707106781187, 0],] 逆操作后的振幅:[[0.707106781187, 0],[0.707106781187, 0],[0, 0],[0, 0],]
测试结论：通过
*/
bool CTestGateCNOT::TestInv(const InitState::type state)
{
    std::ostringstream osh("");
    std::ostringstream osc("");
    std::ostringstream ost("");
    std::ostringstream osprint("");

    int qubits = 2;
    bool density = false;
    SINGLETON(CQuESTIniter)->Init(qubits, (int)state, 0, density);

    CmdExecutor handle;

    {
        osh.str("");
        Cmd cmd;
        cmd.__set_gate("H");
        cmd.targets.push_back(0);
        osh << "h q[" << 0 << "]";
        cmd.desc = osh.str();
        cmd.inverse = false;
        handle.H(cmd);
    }

    std::vector<std::string> initmatrix;
    handle.getAllState(initmatrix);
    osprint << "test CNOT inverse state:" << to_string(state) << " 初始振幅:[";
    for (auto temp : initmatrix)
    {
        osprint << "[" << temp << "],";
    }
    osprint << "] ";

    int controlsize = 1;
    osh.str("");
    osc.str("");
    ost.str("");
    {
        Cmd cmd;
        for (auto i = 0; i < qubits; ++i)
        {
            if (i < controlsize) 
            {
                cmd.controls.push_back(i);
                osh << "c";
                if (osc.str() == "")
                {
                    osc << "q[" << i << "]";
                }
                else
                {
                    osc << ",q[" << i << "]";
                }
            }
            else
            {
                cmd.targets.push_back(i);
                if (ost.str() == "")
                {
                    ost << "q[" << i << "]";
                }
                else
                {
                    ost << ",q[" << i << "]";
                }
            }
        }
        osh << "x";
        cmd.__set_gate("CX");
        cmd.desc = osh.str() + " " + osc.str() + " " + ost.str();
        cmd.inverse = false;
        handle.CNOT(cmd);

        std::ostream& coutos = std::cout;
        cmd.printTo(coutos);
        coutos << std::endl;
    }

    std::vector<std::string> midmatrix;
    handle.getAllState(midmatrix);
    osprint << "应用门后的振幅:[";
    for (auto temp : midmatrix)
    {
        osprint << "[" << temp << "],";
    }
    osprint << "] ";

    osh.str("");
    osc.str("");
    ost.str("");
    {
        Cmd cmd;
        for (auto i = 0; i < qubits; ++i)
        {
            if (i < controlsize) 
            {
                cmd.controls.push_back(i);
                osh << "c";
                if (osc.str() == "")
                {
                    osc << "q[" << i << "]";
                }
                else
                {
                    osc << ",q[" << i << "]";
                }
            }
            else
            {
                cmd.targets.push_back(i);
                if (ost.str() == "")
                {
                    ost << "q[" << i << "]";
                }
                else
                {
                    ost << ",q[" << i << "]";
                }
            }
        }
        osh << "x";
        cmd.__set_gate("CX");
        cmd.desc = osh.str() + " " + osc.str() + " " + ost.str();
        cmd.inverse = true;
        handle.CNOT(cmd);
    }

    std::vector<std::string> endmatrix;
    handle.getAllState(endmatrix);
    osprint << "逆操作后的振幅:[";
    for (auto temp : endmatrix)
    {
        osprint << "[" << temp << "],";
    }
    osprint << "] ";
    std::cout << osprint.str() << std::endl;
    
    SINGLETON(CQuESTIniter)->finalize();
    
    if (endmatrix.size() != initmatrix.size())
    {
        return false;
    }
    for (size_t i = 0; i < endmatrix.size(); ++i)
    {
        if (endmatrix[i] != initmatrix[i])
        {
            return false;
        }
    }

    return true;
}

TEST_F(CTestGateCNOT, TestInv)
{
	EXPECT_TRUE(TestInv(InitState::type::Zero));
    EXPECT_TRUE(TestInv(InitState::type::Plus));
    EXPECT_TRUE(TestInv(InitState::type::Classical));
}
