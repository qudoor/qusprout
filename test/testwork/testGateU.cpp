#include <sstream>
#include <iostream>
#include <fstream>
#include <math.h>
#include "testGateU.h"
#include "common/Singleton.h"
#include "common/QuESTIniter.h"
#include "command/executor.h"

using namespace QS;

CTestGateU::CTestGateU()
{

}

CTestGateU::~CTestGateU()
{

}

/*
测试结果：
Cmd(gate=U, targets=[0], controls=[], rotation=[3.1415926535897931, 1.5707963267948966, 1.0471975511965976], desc=u (3.14159,1.5708,1.0472) q[0], inverse=0)
test U inverse state:Zero 初始振幅:[[0.707106781187, 0],[0.707106781187, 0],] 应用门后的振幅:[[-0.353553390593, -0.612372435696],[0, 0.707106781187],] 逆操作后的振幅:[[0.707106781187, 0],[0.707106781187, 0],] 
Cmd(gate=U, targets=[0], controls=[], rotation=[3.1415926535897931, 1.5707963267948966, 1.0471975511965976], desc=u (3.14159,1.5708,1.0472) q[0], inverse=0)
test U inverse state:Plus 初始振幅:[[1, 0],[0, 0],] 应用门后的振幅:[[0, 0],[0, 1],] 逆操作后的振幅:[[1, 0],[0, 0],] 
Cmd(gate=U, targets=[0], controls=[], rotation=[3.1415926535897931, 1.5707963267948966, 1.0471975511965976], desc=u (3.14159,1.5708,1.0472) q[0], inverse=0)
test U inverse state:Classical 初始振幅:[[0.707106781187, 0],[0.707106781187, 0],] 应用门后的振幅:[[-0.353553390593, -0.612372435696],[0, 0.707106781187],] 逆操作后的振幅:[[0.707106781187, 0],[0.707106781187, 0],] 
测试结论：通过
*/
bool CTestGateU::TestInv(const InitState::type state)
{
    std::ostringstream os("");
    std::ostringstream osprint("");

    int qubits = 1;
    bool density = false;
    SINGLETON(CQuESTIniter)->Init(qubits, (int)state, 0, density);

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
    osprint << "test U inverse state:" << to_string(state) << " 初始振幅:[";
    for (auto temp : initmatrix)
    {
        osprint << "[" << temp << "],";
    }
    osprint << "] ";

    for (auto i = 0; i < qubits; ++i)
    {
        os.str("");
        Cmd cmd;
        cmd.__set_gate("U");
        cmd.targets.push_back(i);
        cmd.rotation.push_back(M_PI);
        cmd.rotation.push_back(M_PI/2);
        cmd.rotation.push_back(M_PI/3);
        os << "u (" << cmd.rotation[0] << "," << cmd.rotation[1] << "," << cmd.rotation[2] << ") q[" << i << "]";
        cmd.desc = os.str();
        cmd.inverse = false;
        handle.U(cmd);

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
        cmd.__set_gate("U");
        cmd.targets.push_back(i);
        cmd.rotation.push_back(M_PI);
        cmd.rotation.push_back(M_PI/2);
        cmd.rotation.push_back(M_PI/3);
        os << "u (" << cmd.rotation[0] << "," << cmd.rotation[1] << "," << cmd.rotation[2] << ") q[" << i << "]";
        cmd.desc = os.str();
        cmd.inverse = true;
        handle.U(cmd);
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

TEST_F(CTestGateU, TestInv)
{
	EXPECT_TRUE(TestInv(InitState::type::Zero));
    EXPECT_TRUE(TestInv(InitState::type::Plus));
    EXPECT_TRUE(TestInv(InitState::type::Classical));
}
