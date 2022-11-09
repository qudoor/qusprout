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
#include "testGateSqrtX.h"
#include "common/Singleton.h"
#include "common/QuESTIniter.h"
#include "command/executor.h"

using namespace QS;

CTestGateSqrtX::CTestGateSqrtX()
{

}

CTestGateSqrtX::~CTestGateSqrtX()
{

}

/*
测试结果：
Cmd(gate=SqrtX, targets=[0], controls=[], rotation=[], desc=sx q[0], inverse=0)
Cmd(gate=SqrtX, targets=[1], controls=[], rotation=[], desc=sx q[1], inverse=0)
test SqrtX inverse state:Zero 初始振幅:[[1, 0],[0, 0],[0, 0],[0, 0],] 应用门后的振幅:[[0, 0.5],[0.5, 0],[0.5, 0],[0, -0.5],] 逆操作后的振幅:[[1, 0],[0, 0],[0, 0],[0, 0],] 
Cmd(gate=SqrtX, targets=[0], controls=[], rotation=[], desc=sx q[0], inverse=0)
Cmd(gate=SqrtX, targets=[1], controls=[], rotation=[], desc=sx q[1], inverse=0)
test SqrtX inverse state:Plus 初始振幅:[[0.5, 0],[0.5, 0],[0.5, 0],[0.5, 0],] 应用门后的振幅:[[0.5, 0],[0.5, 0],[0.5, 0],[0.5, 0],] 逆操作后的振幅:[[0.5, 0],[0.5, 0],[0.5, 0],[0.5, 0],] 
Cmd(gate=SqrtX, targets=[0], controls=[], rotation=[], desc=sx q[0], inverse=0)
Cmd(gate=SqrtX, targets=[1], controls=[], rotation=[], desc=sx q[1], inverse=0)
test SqrtX inverse state:Classical 初始振幅:[[1, 0],[0, 0],[0, 0],[0, 0],] 应用门后的振幅:[[0, 0.5],[0.5, 0],[0.5, 0],[0, -0.5],] 逆操作后的振幅:[[1, 0],[0, 0],[0, 0],[0, 0],] 
测试结论：通过
*/
bool CTestGateSqrtX::TestInv()
{
    std::ostringstream os("");
    std::ostringstream osprint("");

    int qubits = 2;
    bool density = false;
    SINGLETON(CQuESTIniter)->Init(qubits, density);

    CmdExecutor handle;

    std::vector<std::string> initmatrix;
    handle.getAllState(initmatrix);
    osprint << "test SqrtX inverse 初始振幅:[";
    for (auto temp : initmatrix)
    {
        osprint << "[" << temp << "],";
    }
    osprint << "] ";

    for (auto i = 0; i < qubits; ++i)
    {
        os.str("");
        Cmd cmd;
        cmd.__set_gate("SqrtX");
        cmd.targets.push_back(i);
        os << "sx q[" << i << "]";
        cmd.desc = os.str();
        cmd.inverse = false;
        handle.SqrtX(cmd);

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
        cmd.__set_gate("SqrtX");
        cmd.targets.push_back(i);
        os << "sx q[" << i << "]";
        cmd.desc = os.str();
        cmd.inverse = true;
        handle.SqrtX(cmd);
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

TEST_F(CTestGateSqrtX, TestInv)
{
	EXPECT_TRUE(TestInv());
}
