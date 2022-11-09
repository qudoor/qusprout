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

#include <sstream>
#include <iostream>
#include <fstream>
#include <math.h>
#include "testGateU3.h"
#include "common/Singleton.h"
#include "common/QuESTIniter.h"
#include "command/executor.h"

using namespace QS;

CTestGateU3::CTestGateU3()
{

}

CTestGateU3::~CTestGateU3()
{

}

/*
测试结果：
Cmd(gate=U3, targets=[0], controls=[], rotation=[3.1415926535897931, 1.5707963267948966, 1.0471975511965976], desc=u3 (3.14159,1.5708,1.0472) q[0], inverse=0)
test U3 inverse state:Zero 初始振幅:[[1, 0],[0, 0],] 应用门后的振幅:[[0, 0],[0, 1],] 逆操作后的振幅:[[1, 0],[0, 0],] 
Cmd(gate=U3, targets=[0], controls=[], rotation=[3.1415926535897931, 1.5707963267948966, 1.0471975511965976], desc=u3 (3.14159,1.5708,1.0472) q[0], inverse=0)
test U3 inverse state:Plus 初始振幅:[[0.707106781187, 0],[0.707106781187, 0],] 应用门后的振幅:[[-0.353553390593, -0.612372435696],[0, 0.707106781187],] 逆操作后的振幅:[[0.707106781187, 0],[0.707106781187, 0],] 
Cmd(gate=U3, targets=[0], controls=[], rotation=[3.1415926535897931, 1.5707963267948966, 1.0471975511965976], desc=u3 (3.14159,1.5708,1.0472) q[0], inverse=0)
test U3 inverse state:Classical 初始振幅:[[1, 0],[0, 0],] 应用门后的振幅:[[0, 0],[0, 1],] 逆操作后的振幅:[[1, 0],[0, 0],] 
测试结论：通过
*/
bool CTestGateU3::TestInv()
{
    std::ostringstream os("");
    std::ostringstream osprint("");

    int qubits = 1;
    bool density = false;
    SINGLETON(CQuESTIniter)->Init(qubits, density);

    CmdExecutor handle;

    std::vector<std::string> initmatrix;
    handle.getAllState(initmatrix);
    osprint << "test U3 inverse 初始振幅:[";
    for (auto temp : initmatrix)
    {
        osprint << "[" << temp << "],";
    }
    osprint << "] ";

    for (auto i = 0; i < qubits; ++i)
    {
        os.str("");
        Cmd cmd;
        cmd.__set_gate("U3");
        cmd.targets.push_back(i);
        cmd.rotation.push_back(M_PI);
        cmd.rotation.push_back(M_PI/2);
        cmd.rotation.push_back(M_PI/3);
        os << "u3 (" << cmd.rotation[0] << "," << cmd.rotation[1] << "," << cmd.rotation[2] << ") q[" << i << "]";
        cmd.desc = os.str();
        cmd.inverse = false;
        handle.U3(cmd);

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
        cmd.__set_gate("U3");
        cmd.targets.push_back(i);
        cmd.rotation.push_back(M_PI);
        cmd.rotation.push_back(M_PI/2);
        cmd.rotation.push_back(M_PI/3);
        os << "u3 (" << cmd.rotation[0] << "," << cmd.rotation[1] << "," << cmd.rotation[2] << ") q[" << i << "]";
        cmd.desc = os.str();
        cmd.inverse = true;
        handle.U3(cmd);
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

bool CTestGateU3::VerifyXAndU3()
{
    std::ostringstream os("");
    std::ostringstream osprint("");

    int qubits = 1;
    bool density = false;
    SINGLETON(CQuESTIniter)->Init(qubits, density);

    CmdExecutor handle;

    std::vector<std::string> initmatrix;
    handle.getAllState(initmatrix);
    osprint << "test X<===>U3 inverse 初始振幅:[";
    for (auto temp : initmatrix)
    {
        osprint << "[" << temp << "],";
    }
    osprint << "] ";

    for (auto i = 0; i < qubits; ++i)
    {
        os.str("");
        Cmd cmd;
        cmd.__set_gate("X");
        cmd.targets.push_back(i);
        os << "x q[" << i << "]";
        cmd.desc = os.str();
        cmd.inverse = false;
        handle.X(cmd);

        std::ostream& coutos = std::cout;
        cmd.printTo(coutos);
        coutos << std::endl;
    }

    std::vector<std::string> midmatrix;
    handle.getAllState(midmatrix);
    osprint << "应用X门后的振幅:[";
    for (auto temp : midmatrix)
    {
        osprint << "[" << temp << "],";
    }
    osprint << "] ";

    SINGLETON(CQuESTIniter)->finalize();

    SINGLETON(CQuESTIniter)->Init(qubits, density);

    CmdExecutor handle1;

    std::vector<std::string> reinitmatrix;
    handle1.getAllState(reinitmatrix);
    osprint << "重新初始化后的振幅:[";
    for (auto temp : reinitmatrix)
    {
        osprint << "[" << temp << "],";
    }
    osprint << "] ";

    for (auto i = 0; i < qubits; ++i)
    {
        os.str("");
        Cmd cmd;
        cmd.__set_gate("U3");
        cmd.targets.push_back(i);
        cmd.rotation.push_back(M_PI);
        cmd.rotation.push_back(0);
        cmd.rotation.push_back(M_PI);
        os << "u3 (" << cmd.rotation[0] << "," << cmd.rotation[1] << "," << cmd.rotation[2] << ") q[" << i << "]";
        cmd.desc = os.str();
        cmd.inverse = false;
        handle1.U3(cmd);
    }

    std::vector<std::string> endmatrix;
    handle1.getAllState(endmatrix);
    osprint << "应用U3(pi,0,pi)后的振幅:[";
    for (auto temp : endmatrix)
    {
        osprint << "[" << temp << "],";
    }
    osprint << "] ";
    std::cout << osprint.str() << std::endl;
    
    SINGLETON(CQuESTIniter)->finalize();
    
    if (endmatrix.size() != midmatrix.size())
    {
        return false;
    }
    for (size_t i = 0; i < endmatrix.size(); ++i)
    {
        if (endmatrix[i] != midmatrix[i])
        {
            return false;
        }
    }

    return true;
}

bool CTestGateU3::VerifyRXAndU3()
{
    std::ostringstream os("");
    std::ostringstream osprint("");

    int qubits = 1;
    bool density = false;
    SINGLETON(CQuESTIniter)->Init(qubits, density);

    CmdExecutor handle;

    std::vector<std::string> initmatrix;
    handle.getAllState(initmatrix);
    osprint << "test RX<===>U3 inverse 初始振幅:[";
    for (auto temp : initmatrix)
    {
        osprint << "[" << temp << "],";
    }
    osprint << "] ";

    auto x = M_PI/3;
    for (auto i = 0; i < qubits; ++i)
    {
        os.str("");
        Cmd cmd;
        cmd.__set_gate("Rx");
        cmd.targets.push_back(i);
        cmd.rotation.push_back(x);
        os << "rx (" << cmd.rotation[0] << ") q[" << i << "]";
        cmd.desc = os.str();
        cmd.inverse = false;
        handle.Rx(cmd);
    }

    std::vector<std::string> midmatrix;
    handle.getAllState(midmatrix);
    osprint << "应用RX门后的振幅:[";
    for (auto temp : midmatrix)
    {
        osprint << "[" << temp << "],";
    }
    osprint << "] ";

    SINGLETON(CQuESTIniter)->finalize();

    SINGLETON(CQuESTIniter)->Init(qubits, density);

    CmdExecutor handle1;

    std::vector<std::string> reinitmatrix;
    handle1.getAllState(reinitmatrix);
    osprint << "重新初始化后的振幅:[";
    for (auto temp : reinitmatrix)
    {
        osprint << "[" << temp << "],";
    }
    osprint << "] ";

    for (auto i = 0; i < qubits; ++i)
    {
        os.str("");
        Cmd cmd;
        cmd.__set_gate("U3");
        cmd.targets.push_back(i);
        cmd.rotation.push_back(x);
        cmd.rotation.push_back(-M_PI/2);
        cmd.rotation.push_back(M_PI/2);
        os << "u3 (" << cmd.rotation[0] << "," << cmd.rotation[1] << "," << cmd.rotation[2] << ") q[" << i << "]";
        cmd.desc = os.str();
        cmd.inverse = false;
        handle1.U3(cmd);
    }

    std::vector<std::string> endmatrix;
    handle1.getAllState(endmatrix);
    osprint << "应用U3(x,-pi/2,pi/2)后的振幅:[";
    for (auto temp : endmatrix)
    {
        osprint << "[" << temp << "],";
    }
    osprint << "] ";
    std::cout << osprint.str() << std::endl;
    
    SINGLETON(CQuESTIniter)->finalize();
    
    if (endmatrix.size() != midmatrix.size())
    {
        return false;
    }
    for (size_t i = 0; i < endmatrix.size(); ++i)
    {
        if (endmatrix[i] != midmatrix[i])
        {
            return false;
        }
    }

    return true;
}

TEST_F(CTestGateU3, TestInv)
{
	EXPECT_TRUE(TestInv());
}

TEST_F(CTestGateU3, VerifyXAndU3)
{
	EXPECT_TRUE(VerifyXAndU3());
}

TEST_F(CTestGateU3, VerifyRXAndU3)
{
	EXPECT_TRUE(VerifyRXAndU3());
}