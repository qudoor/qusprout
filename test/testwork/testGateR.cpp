#include <sstream>
#include <iostream>
#include <fstream>
#include <math.h>
#include "testGateR.h"
#include "common/Singleton.h"
#include "common/QuESTIniter.h"
#include "command/executor.h"

using namespace QS;

CTestGateR::CTestGateR()
{

}

CTestGateR::~CTestGateR()
{

}

/*
测试结果：
Cmd(gate=Z, targets=[0], controls=[], rotation=[], desc=z q[0], inverse=0)
Cmd(gate=Z, targets=[1], controls=[], rotation=[], desc=z q[1], inverse=0)
Cmd(gate=R, targets=[0], controls=[], rotation=[1.5707963267948966, 1.5707963267948966], desc=r (1.5708,1.5708) q[0], inverse=0)
Cmd(gate=R, targets=[1], controls=[], rotation=[1.5707963267948966, 1.5707963267948966], desc=r (1.5708,1.5708) q[1], inverse=0)
test R inverse state:Zero 初始振幅:[[1, 0],[0, 0],[0, 0],[0, 0],] 应用门后的振幅:[[0.5, 0],[0.5, 0],[0.5, 0],[0.5, 0],] 逆操作后的振幅:[[1, 0],[0, 0],[0, 0],[0, 0],] 
Cmd(gate=Z, targets=[0], controls=[], rotation=[], desc=z q[0], inverse=0)
Cmd(gate=Z, targets=[1], controls=[], rotation=[], desc=z q[1], inverse=0)
Cmd(gate=R, targets=[0], controls=[], rotation=[1.5707963267948966, 1.5707963267948966], desc=r (1.5708,1.5708) q[0], inverse=0)
Cmd(gate=R, targets=[1], controls=[], rotation=[1.5707963267948966, 1.5707963267948966], desc=r (1.5708,1.5708) q[1], inverse=0)
test R inverse state:Plus 初始振幅:[[0.5, 0],[-0.5, 0],[-0.5, 0],[0.5, 0],] 应用门后的振幅:[[1, 0],[0, 0],[0, 0],[0, 0],] 逆操作后的振幅:[[0.5, 0],[-0.5, 0],[-0.5, 0],[0.5, 0],] 
Cmd(gate=Z, targets=[0], controls=[], rotation=[], desc=z q[0], inverse=0)
Cmd(gate=Z, targets=[1], controls=[], rotation=[], desc=z q[1], inverse=0)
Cmd(gate=R, targets=[0], controls=[], rotation=[1.5707963267948966, 1.5707963267948966], desc=r (1.5708,1.5708) q[0], inverse=0)
Cmd(gate=R, targets=[1], controls=[], rotation=[1.5707963267948966, 1.5707963267948966], desc=r (1.5708,1.5708) q[1], inverse=0)
test R inverse state:Classical 初始振幅:[[1, 0],[0, 0],[0, 0],[0, 0],] 应用门后的振幅:[[0.5, 0],[0.5, 0],[0.5, 0],[0.5, 0],] 逆操作后的振幅:[[1, 0],[0, 0],[0, 0],[0, 0],] 
测试结论：通过,需要修改getAllState获取double的精度
*/
bool CTestGateR::TestInv(const InitState::type state)
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

    std::vector<std::string> initmatrix;
    handle.getAllState(initmatrix);
    osprint << "test R inverse state:" << to_string(state) << " 初始振幅:[";
    for (auto temp : initmatrix)
    {
        osprint << "[" << temp << "],";
    }
    osprint << "] ";

    for (auto i = 0; i < qubits; ++i)
    {
        os.str("");
        Cmd cmd;
        cmd.__set_gate("R");
        cmd.targets.push_back(i);
        cmd.rotation.push_back(M_PI/2);
        cmd.rotation.push_back(M_PI/2);
        os << "r (" << cmd.rotation[0] << "," << cmd.rotation[1] << ") q[" << i << "]";
        cmd.desc = os.str();
        cmd.inverse = false;
        handle.R(cmd);

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
        cmd.__set_gate("R");
        cmd.targets.push_back(i);
        cmd.rotation.push_back(M_PI/2);
        cmd.rotation.push_back(M_PI/2);
        os << "r (" << cmd.rotation[0] << "," << cmd.rotation[1] << ") q[" << i << "]";
        cmd.desc = os.str();
        cmd.inverse = true;
        handle.R(cmd);
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
        if (initmatrix[i].find("-0,") == 0)
        {
            initmatrix[i] = initmatrix[i].substr(1);
        }
        else
        {
            size_t endlen = initmatrix[i].length();
            if (endlen >= 4)
            {
                std::string temp = initmatrix[i].substr(0, endlen-4);
                std::string temp1 = initmatrix[i].substr(endlen-4);
                if (temp1 == ", -0")
                {
                    initmatrix[i] = temp + ", 0";
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

TEST_F(CTestGateR, TestInv)
{
	EXPECT_TRUE(TestInv(InitState::type::Zero));
    EXPECT_TRUE(TestInv(InitState::type::Plus));
    EXPECT_TRUE(TestInv(InitState::type::Classical));
}
