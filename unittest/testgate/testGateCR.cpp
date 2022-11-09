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
#include "testGateCR.h"
#include "common/Singleton.h"
#include "common/QuESTIniter.h"
#include "command/executor.h"

using namespace QS;

CTestGateCR::CTestGateCR()
{

}

CTestGateCR::~CTestGateCR()
{

}

/*
测试结果：
Cmd(gate=CR, targets=[0, 1], controls=[], rotation=[1.5707963267948966], desc=cr (1.5708) q[ 0 ],q[ 1 ], inverse=0)
test CR inverse state:Zero 初始振幅:[[0.5, 0],[0.5, 0],[0.5, 0],[0.5, 0],] 应用门后的振幅:[[0.5, 0],[0.5, 0],[0.5, 0],[0, 0.5],] 逆操作后的振幅:[[0.5, 0],[0.5, 0],[0.5, 0],[0.5, 0],] 
Cmd(gate=CR, targets=[0, 1], controls=[], rotation=[1.5707963267948966], desc=cr (1.5708) q[ 0 ],q[ 1 ], inverse=0)
test CR inverse state:Plus 初始振幅:[[1, 0],[0, 0],[0, 0],[0, 0],] 应用门后的振幅:[[1, 0],[0, 0],[0, 0],[0, 0],] 逆操作后的振幅:[[1, 0],[0, 0],[0, 0],[0, 0],] 
Cmd(gate=CR, targets=[0, 1], controls=[], rotation=[1.5707963267948966], desc=cr (1.5708) q[ 0 ],q[ 1 ], inverse=0)
test CR inverse state:Classical 初始振幅:[[0.5, 0],[0.5, 0],[0.5, 0],[0.5, 0],] 应用门后的振幅:[[0.5, 0],[0.5, 0],[0.5, 0],[0, 0.5],] 逆操作后的振幅:[[0.5, 0],[0.5, 0],[0.5, 0],[0.5, 0],]
测试结论：通过
*/
bool CTestGateCR::TestInv()
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
    osprint << "test CR inverse 初始振幅:[";
    for (auto temp : initmatrix)
    {
        osprint << "[" << temp << "],";
    }
    osprint << "] ";

    {
        os.str("");
        Cmd cmd;
        cmd.__set_gate("CR");
        cmd.rotation.push_back(M_PI/2);
        cmd.targets.push_back(0);
        cmd.targets.push_back(1);
        os << "cr (" << cmd.rotation[0] << ") q[ 0 ],q[ 1 ]";
        cmd.desc = os.str();
        cmd.inverse = false;
        handle.CR(cmd);

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

    {
        os.str("");
        Cmd cmd;
        cmd.__set_gate("CR");
        cmd.rotation.push_back(M_PI/2);
        cmd.targets.push_back(0);
        cmd.targets.push_back(1);
        os << "cr (" << cmd.rotation[0] << ") q[ 0 ],q[ 1 ]";
        cmd.desc = os.str();
        cmd.inverse = true;
        handle.CR(cmd);
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

TEST_F(CTestGateCR, TestInv)
{
	EXPECT_TRUE(TestInv());
}
