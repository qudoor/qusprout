#include <sstream>
#include <iostream>
#include <fstream>
#include <math.h>
#include "testGateCU3.h"
#include "common/Singleton.h"
#include "common/QuESTIniter.h"
#include "command/executor.h"

using namespace QS;

CTestGateCU3::CTestGateCU3()
{

}

CTestGateCU3::~CTestGateCU3()
{

}

/*
测试结果：
Cmd(gate=CU3, targets=[0, 1], controls=[], rotation=[3.1415926535897931, 1.5707963267948966, 1.0471975511965976], desc=cu3 (3.14159,1.5708,1.0472) q[ 0 ],q[ 1 ], inverse=0)
Cmd(gate=CU3, targets=[0, 1], controls=[], rotation=[3.1415926535897931, 1.5707963267948966, 1.0471975511965976], desc=cu3 (3.14159,1.5708,1.0472) q[ 0 ],q[ 1 ], inverse=0)
test CU3 inverse state:Zero 初始振幅:[[0.5, 0],[0.5, 0],[0.5, 0],[0.5, 0],] 应用门后的振幅:[[0.5, 0],[0.433012701892, -0.25],[0.5, 0],[0.433012701892, -0.25],] 逆操作后的振幅:[[0.5, 0],[0.5, 0],[0.5, 0],[0.5, 0],] 
Cmd(gate=CU3, targets=[0, 1], controls=[], rotation=[3.1415926535897931, 1.5707963267948966, 1.0471975511965976], desc=cu3 (3.14159,1.5708,1.0472) q[ 0 ],q[ 1 ], inverse=0)
Cmd(gate=CU3, targets=[0, 1], controls=[], rotation=[3.1415926535897931, 1.5707963267948966, 1.0471975511965976], desc=cu3 (3.14159,1.5708,1.0472) q[ 0 ],q[ 1 ], inverse=0)
test CU3 inverse state:Plus 初始振幅:[[1, 0],[0, 0],[0, 0],[0, 0],] 应用门后的振幅:[[1, 0],[0, 0],[0, 0],[0, 0],] 逆操作后的振幅:[[1, 0],[0, 0],[0, 0],[0, 0],] 
Cmd(gate=CU3, targets=[0, 1], controls=[], rotation=[3.1415926535897931, 1.5707963267948966, 1.0471975511965976], desc=cu3 (3.14159,1.5708,1.0472) q[ 0 ],q[ 1 ], inverse=0)
Cmd(gate=CU3, targets=[0, 1], controls=[], rotation=[3.1415926535897931, 1.5707963267948966, 1.0471975511965976], desc=cu3 (3.14159,1.5708,1.0472) q[ 0 ],q[ 1 ], inverse=0)
test CU3 inverse state:Classical 初始振幅:[[0.5, 0],[0.5, 0],[0.5, 0],[0.5, 0],] 应用门后的振幅:[[0.5, 0],[0.433012701892, -0.25],[0.5, 0],[0.433012701892, -0.25],] 逆操作后的振幅:[[0.5, 0],[0.5, 0],[0.5, 0],[0.5, 0],] 
测试结论：通过
*/
bool CTestGateCU3::TestInv()
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
    osprint << "test CU3 inverse 初始振幅:[";
    for (auto temp : initmatrix)
    {
        osprint << "[" << temp << "],";
    }
    osprint << "] ";

    for (auto i = 0; i < qubits; ++i)
    {
        os.str("");
        Cmd cmd;
        cmd.__set_gate("CU3");
        cmd.rotation.push_back(M_PI);
        cmd.rotation.push_back(M_PI/2);
        cmd.rotation.push_back(M_PI/3);
        cmd.targets.push_back(0);
        cmd.targets.push_back(1);
        os << "cu3 (" << cmd.rotation[0] << "," << cmd.rotation[1] << "," << cmd.rotation[2] << ") q[ 0 ],q[ 1 ]";
        cmd.desc = os.str();
        cmd.inverse = false;
        handle.CU3(cmd);

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
        cmd.__set_gate("CU3");
        cmd.rotation.push_back(M_PI);
        cmd.rotation.push_back(M_PI/2);
        cmd.rotation.push_back(M_PI/3);
        cmd.targets.push_back(0);
        cmd.targets.push_back(1);
        os << "cu3 (" << cmd.rotation[0] << "," << cmd.rotation[1] << "," << cmd.rotation[2] << ") q[ 0 ],q[ 1 ]";
        cmd.desc = os.str();
        cmd.inverse = true;
        handle.CU3(cmd);
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

TEST_F(CTestGateCU3, TestInv)
{
	EXPECT_TRUE(TestInv());
}
