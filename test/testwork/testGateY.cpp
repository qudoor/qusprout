#include <sstream>
#include <iostream>
#include <fstream>
#include "testGateY.h"
#include "common/Singleton.h"
#include "common/QuESTIniter.h"
#include "command/executor.h"

using namespace QS;

CTestGateY::CTestGateY()
{

}

CTestGateY::~CTestGateY()
{

}

/*
测试结果：
Cmd(gate=Y, targets=[0], controls=[], rotation=[], desc=y q[0], inverse=0)
Cmd(gate=Y, targets=[1], controls=[], rotation=[], desc=y q[1], inverse=0)
test Y inverse state:Zero 初始振幅:[[1, 0],[0, 0],[0, 0],[0, 0],] 应用门后的振幅:[[-0, -0],[0, 0],[0, 0],[-1, -0],] 逆操作后的振幅:[[1, 0],[0, 0],[0, 0],[0, 0],] 
Cmd(gate=Y, targets=[0], controls=[], rotation=[], desc=y q[0], inverse=0)
Cmd(gate=Y, targets=[1], controls=[], rotation=[], desc=y q[1], inverse=0)
test Y inverse state:Plus 初始振幅:[[0.5, 0],[0.5, 0],[0.5, 0],[0.5, 0],] 应用门后的振幅:[[-0.5, -0],[0.5, 0],[0.5, 0],[-0.5, -0],] 逆操作后的振幅:[[0.5, 0],[0.5, 0],[0.5, 0],[0.5, 0],] 
Cmd(gate=Y, targets=[0], controls=[], rotation=[], desc=y q[0], inverse=0)
Cmd(gate=Y, targets=[1], controls=[], rotation=[], desc=y q[1], inverse=0)
test Y inverse state:Classical 初始振幅:[[1, 0],[0, 0],[0, 0],[0, 0],] 应用门后的振幅:[[-0, -0],[0, 0],[0, 0],[-1, -0],] 逆操作后的振幅:[[1, 0],[0, 0],[0, 0],[0, 0],] 
测试结论：通过
*/
bool CTestGateY::TestInv(const InitState::type state)
{
    std::ostringstream os("");
    std::ostringstream osprint("");

    int qubits = 2;
    bool density = false;
    SINGLETON(CQuESTIniter)->Init(qubits, (int)state, 0, density);

    CmdExecutor handle;

    std::vector<std::string> initmatrix;
    handle.getAllState(initmatrix);
    osprint << "test Y inverse state:" << to_string(state) << " 初始振幅:[";
    for (auto temp : initmatrix)
    {
        osprint << "[" << temp << "],";
    }
    osprint << "] ";

    for (auto i = 0; i < qubits; ++i)
    {
        os.str("");
        Cmd cmd;
        cmd.__set_gate("Y");
        cmd.targets.push_back(i);
        os << "y q[" << i << "]";
        cmd.desc = os.str();
        cmd.inverse = false;
        handle.Y(cmd);

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

    for (auto i = 0; i < qubits; ++i)
    {
        os.str("");
        Cmd cmd;
        cmd.__set_gate("Y");
        cmd.targets.push_back(i);
        os << "y q[" << i << "]";
        cmd.desc = os.str();
        cmd.inverse = true;
        handle.Y(cmd);
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

TEST_F(CTestGateY, TestInv)
{
	EXPECT_TRUE(TestInv(InitState::type::Zero));
    EXPECT_TRUE(TestInv(InitState::type::Plus));
    EXPECT_TRUE(TestInv(InitState::type::Classical));
}