#include <sstream>
#include <iostream>
#include <fstream>
#include <math.h>
#include "testGateP.h"
#include "common/Singleton.h"
#include "common/QuESTIniter.h"
#include "command/executor.h"

using namespace QS;

CTestGateP::CTestGateP()
{

}

CTestGateP::~CTestGateP()
{

}

/*
测试结果：
Cmd(gate=P, targets=[0], controls=[], rotation=[1.5707963267948966], desc=p ( 1.5708 ) q[0], inverse=0)
Cmd(gate=P, targets=[1], controls=[], rotation=[1.5707963267948966], desc=p ( 1.5708 ) q[1], inverse=0)
test P inverse state:Zero 初始振幅:[[0.5, 0],[0.5, 0],[0.5, 0],[0.5, 0],] 应用门后的振幅:[[0.5, 0],[3.06161699787e-17, 0.5],[3.06161699787e-17, 0.5],[-0.5, 6.12323399574e-17],] 逆操作后的振幅:[[0.5, 0],[0.5, 0],[0.5, 0],[0.5, 0],] 
Cmd(gate=P, targets=[0], controls=[], rotation=[1.5707963267948966], desc=p ( 1.5708 ) q[0], inverse=0)
Cmd(gate=P, targets=[1], controls=[], rotation=[1.5707963267948966], desc=p ( 1.5708 ) q[1], inverse=0)
test P inverse state:Plus 初始振幅:[[1, 0],[0, 0],[0, 0],[0, 0],] 应用门后的振幅:[[1, 0],[0, 0],[0, 0],[0, 0],] 逆操作后的振幅:[[1, 0],[0, 0],[0, 0],[0, 0],] 
Cmd(gate=P, targets=[0], controls=[], rotation=[1.5707963267948966], desc=p ( 1.5708 ) q[0], inverse=0)
Cmd(gate=P, targets=[1], controls=[], rotation=[1.5707963267948966], desc=p ( 1.5708 ) q[1], inverse=0)
test P inverse state:Classical 初始振幅:[[0.5, 0],[0.5, 0],[0.5, 0],[0.5, 0],] 应用门后的振幅:[[0.5, 0],[3.06161699787e-17, 0.5],[3.06161699787e-17, 0.5],[-0.5, 6.12323399574e-17],] 逆操作后的振幅:[[0.5, 0],[0.5, 0],[0.5, 0],[0.5, 0],] 
测试结论：不确定
*/
bool CTestGateP::TestInv()
{
    std::ostringstream os("");
    std::ostringstream osprint("");

    int qubits = 2;
    bool density = false;
    SINGLETON(CQuESTIniter)->Init(qubits, density);

    CmdExecutor handle;

    os.str("");
    for (auto i = 0; i < qubits; ++i)
    {
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
    osprint << "test P inverse 初始振幅:[";
    for (auto temp : initmatrix)
    {
        osprint << "[" << temp << "],";
    }
    osprint << "] ";

    for (auto i = 0; i < qubits; ++i)
    {
        os.str("");
        Cmd cmd;
        cmd.__set_gate("P");
        cmd.targets.push_back(i);
        cmd.rotation.push_back(M_PI/2);
        os << "p ( " << cmd.rotation[0] << " ) q[" << i << "]";
        cmd.desc = os.str();
        cmd.inverse = false;
        handle.P(cmd);

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
        cmd.__set_gate("P");
        cmd.targets.push_back(i);
        cmd.rotation.push_back(M_PI/2);
        os << "p ( " << cmd.rotation[0] << " ) q[" << i << "]";
        cmd.desc = os.str();
        cmd.inverse = true;
        handle.P(cmd);
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

TEST_F(CTestGateP, TestInv)
{
	EXPECT_TRUE(TestInv());
}
