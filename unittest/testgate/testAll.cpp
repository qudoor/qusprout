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
#include <stdlib.h>
#include "testAll.h"
#include "common/Singleton.h"
#include "common/QuESTIniter.h"
#include "command/executor.h"

using namespace QS;

CTestAll::CTestAll()
{

}

CTestAll::~CTestAll()
{

}

bool CTestAll::TestInv()
{
    std::ostringstream os("");
    std::ostringstream osprint("");

    int qubits = 2;
    bool density = false;
    SINGLETON(CQuESTIniter)->Init(qubits, density);

    CmdExecutor handle;

    for (auto i = 1; i < qubits; ++i)
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
    osprint << "test reset 初始振幅:[";
    for (auto temp : initmatrix)
    {
        osprint << "[" << temp << "],";
    }
    osprint << "] ";

    {
        std::vector<int32_t> resetqubits;
        resetqubits.push_back(0);
        handle.resetQubits(resetqubits);
    }

    std::vector<std::string> midmatrix;
    handle.getAllState(midmatrix);
    osprint << "应用门后的振幅:[";
    for (auto temp : midmatrix)
    {
        osprint << "[" << temp << "],";
    }
    osprint << "] ";

    std::cout << osprint.str() << std::endl;

    return true;
}

TEST_F(CTestAll, TestInv)
{
	EXPECT_TRUE(TestInv());
}
