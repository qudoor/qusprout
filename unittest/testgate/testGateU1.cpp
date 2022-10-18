#include <sstream>
#include <iostream>
#include <fstream>
#include <math.h>
#include "testGateU1.h"
#include "common/Singleton.h"
#include "common/QuESTIniter.h"
#include "command/executor.h"

using namespace QS;

CTestGateU1::CTestGateU1()
{

}

CTestGateU1::~CTestGateU1()
{

}

/*
测试结果：
Cmd(gate=U1, targets=[0], controls=[], rotation=[1.5707963267948966], desc=u1 (1.5708) q[0], inverse=0)
Cmd(gate=U1, targets=[1], controls=[], rotation=[1.5707963267948966], desc=u1 (1.5708) q[1], inverse=0)
test U1 inverse state:Zero 初始振幅:[[0.707106781187, 0],[0.707106781187, 0],[0, 0],[0, 0],] 应用门后的振幅:[[0.707106781187, 0],[4.32978028118e-17, 0.707106781187],[0, 0],[0, 0],] 逆操作后的振幅:[[0.707106781187, 0],[0.707106781187, 0],[0, 0],[0, 0],] 
Cmd(gate=U1, targets=[0], controls=[], rotation=[1.5707963267948966], desc=u1 (1.5708) q[0], inverse=0)
Cmd(gate=U1, targets=[1], controls=[], rotation=[1.5707963267948966], desc=u1 (1.5708) q[1], inverse=0)
test U1 inverse state:Plus 初始振幅:[[0.707106781187, 0],[0, 0],[0.707106781187, 0],[0, 0],] 应用门后的振幅:[[0.707106781187, 0],[0, 0],[4.32978028118e-17, 0.707106781187],[0, 0],] 逆操作后的振幅:[[0.707106781187, 0],[0, 0],[0.707106781187, 0],[0, 0],] 
Cmd(gate=U1, targets=[0], controls=[], rotation=[1.5707963267948966], desc=u1 (1.5708) q[0], inverse=0)
Cmd(gate=U1, targets=[1], controls=[], rotation=[1.5707963267948966], desc=u1 (1.5708) q[1], inverse=0)
test U1 inverse state:Classical 初始振幅:[[0.707106781187, 0],[0.707106781187, 0],[0, 0],[0, 0],] 应用门后的振幅:[[0.707106781187, 0],[4.32978028118e-17, 0.707106781187],[0, 0],[0, 0],] 逆操作后的振幅:[[0.707106781187, 0],[0.707106781187, 0],[0, 0],[0, 0],]
测试结论：通过
*/
bool CTestGateU1::TestInv()
{
    std::ostringstream os("");
    std::ostringstream osprint("");

    int qubits = 2;
    bool density = false;
    SINGLETON(CQuESTIniter)->Init(qubits, density);

    CmdExecutor handle;

    {
        os.str("");
        Cmd cmd;
        cmd.__set_gate("H");
        cmd.targets.push_back(0);
        os << "h q[" << 0 << "]";
        cmd.desc = os.str();
        cmd.inverse = false;
        handle.H(cmd);
    }

    std::vector<std::string> initmatrix;
    handle.getAllState(initmatrix);
    osprint << "test U1 inverse 初始振幅:[";
    for (auto temp : initmatrix)
    {
        osprint << "[" << temp << "],";
    }
    osprint << "] ";

    for (auto i = 0; i < qubits; ++i)
    {
        os.str("");
        Cmd cmd;
        cmd.__set_gate("U1");
        cmd.targets.push_back(i);
        cmd.rotation.push_back(M_PI/2);
        os << "u1 (" << cmd.rotation[0] << ") q[" << i << "]";
        cmd.desc = os.str();
        cmd.inverse = false;
        handle.U1(cmd);

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
        cmd.__set_gate("U1");
        cmd.targets.push_back(i);
        cmd.rotation.push_back(M_PI/2);
        os << "u1 (" << cmd.rotation[0] << ") q[" << i << "]";
        cmd.desc = os.str();
        cmd.inverse = true;
        handle.U1(cmd);
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

TEST_F(CTestGateU1, TestInv)
{
	EXPECT_TRUE(TestInv());
}
