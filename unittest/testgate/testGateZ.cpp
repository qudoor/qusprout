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
#include "testGateZ.h"
#include "common/Singleton.h"
#include "common/QuESTIniter.h"
#include "command/executor.h"

using namespace QS;

CTestGateZ::CTestGateZ()
{

}

CTestGateZ::~CTestGateZ()
{

}

/*
测试结果：
Cmd(gate=Z, targets=[0], controls=[], rotation=[], desc=z q[0], inverse=0)
Cmd(gate=Z, targets=[1], controls=[], rotation=[], desc=z q[1], inverse=0)
test Z inverse state:Zero 初始振幅:[[1, 0],[0, 0],[0, 0],[0, 0],] 应用门后的振幅:[[1, 0],[-0, 0],[-0, 0],[0, -0],] 逆操作后的振幅:[[1, 0],[0, -0],[0, -0],[-0, 0],] 
Cmd(gate=Z, targets=[0], controls=[], rotation=[], desc=z q[0], inverse=0)
Cmd(gate=Z, targets=[1], controls=[], rotation=[], desc=z q[1], inverse=0)
test Z inverse state:Plus 初始振幅:[[0.5, 0],[0.5, 0],[0.5, 0],[0.5, 0],] 应用门后的振幅:[[0.5, 0],[-0.5, 0],[-0.5, 0],[0.5, -0],] 逆操作后的振幅:[[0.5, 0],[0.5, -0],[0.5, -0],[0.5, -0],] 
Cmd(gate=Z, targets=[0], controls=[], rotation=[], desc=z q[0], inverse=0)
Cmd(gate=Z, targets=[1], controls=[], rotation=[], desc=z q[1], inverse=0)
test Z inverse state:Classical 初始振幅:[[1, 0],[0, 0],[0, 0],[0, 0],] 应用门后的振幅:[[1, 0],[-0, 0],[-0, 0],[0, -0],] 逆操作后的振幅:[[1, 0],[0, -0],[0, -0],[-0, 0],] 
测试结论：通过
*/
bool CTestGateZ::TestInv()
{
    std::ostringstream os("");
    std::ostringstream osprint("");

    int qubits = 2;
    bool density = false;
    SINGLETON(CQuESTIniter)->Init(qubits, density);

    CmdExecutor handle;

    std::vector<std::string> initmatrix;
    handle.getAllState(initmatrix);
    osprint << "test Z inverse 初始振幅:[";
    for (auto temp : initmatrix)
    {
        osprint << "[" << temp << "],";
    }
    osprint << "] ";

    for (auto i = 0; i < qubits; ++i)
    {
        os.str("");
        Cmd cmd;
        cmd.__set_gate("Z");
        cmd.targets.push_back(i);
        os << "z q[" << i << "]";
        cmd.desc = os.str();
        cmd.inverse = false;
        handle.Z(cmd);

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
        cmd.__set_gate("Z");
        cmd.targets.push_back(i);
        os << "z q[" << i << "]";
        cmd.desc = os.str();
        cmd.inverse = true;
        handle.Z(cmd);
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
        if (endmatrix[i].find("-0,") == 0)
        {
            endmatrix[i] = endmatrix[i].substr(1);
        }
        else
        {
            size_t endlen = endmatrix[i].length();
            if (endlen >= 4)
            {
                std::string temp = endmatrix[i].substr(0, endlen-4);
                std::string temp1 = endmatrix[i].substr(endlen-4);
                if (temp1 == ", -0")
                {
                    endmatrix[i] = temp + ", 0";
                }
            }
        }

        if (endmatrix[i] != initmatrix[i])
        {
            return false;
        }
    }

    return true;
}

TEST_F(CTestGateZ, TestInv)
{
	EXPECT_TRUE(TestInv());
}
