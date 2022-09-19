#include <sstream>
#include <iostream>
#include <fstream>
#include <math.h>
#include "testGateRZZ.h"
#include "common/Singleton.h"
#include "common/QuESTIniter.h"
#include "command/executor.h"

using namespace QS;

CTestGateRZZ::CTestGateRZZ()
{

}

CTestGateRZZ::~CTestGateRZZ()
{

}

/*
测试结果：
Cmd(gate=Rzz, targets=[0, 1], controls=[], rotation=[1.5707963267948966], desc=rzz (1.5708) q[ 0 ],q[ 1 ], inverse=0)
test RZZ inverse state:Zero 初始振幅:[[1, 0],[0, 0],[0, 0],[0, 0],] 应用门后的振幅:[[0.707106781187, -0.707106781187],[0, 0],[0, 0],[0, 0],] 逆操作后的振幅:[[1, 0],[0, 0],[0, 0],[0, 0],] 
Cmd(gate=Rzz, targets=[0, 1], controls=[], rotation=[1.5707963267948966], desc=rzz (1.5708) q[ 0 ],q[ 1 ], inverse=0)
test RZZ inverse state:Plus 初始振幅:[[0.5, 0],[0.5, 0],[0.5, 0],[0.5, 0],] 应用门后的振幅:[[0.353553390593, -0.353553390593],[0.353553390593, 0.353553390593],[0.353553390593, 0.353553390593],[0.353553390593, -0.353553390593],] 逆操作后的振幅:[[0.5, 0],[0.5, 0],[0.5, 0],[0.5, 0],] 
Cmd(gate=Rzz, targets=[0, 1], controls=[], rotation=[1.5707963267948966], desc=rzz (1.5708) q[ 0 ],q[ 1 ], inverse=0)
test RZZ inverse state:Classical 初始振幅:[[1, 0],[0, 0],[0, 0],[0, 0],] 应用门后的振幅:[[0.707106781187, -0.707106781187],[0, 0],[0, 0],[0, 0],] 逆操作后的振幅:[[1, 0],[0, 0],[0, 0],[0, 0],]
测试结论：通过
*/
bool CTestGateRZZ::TestInv(const InitState::type state)
{
    std::ostringstream os("");
    std::ostringstream osprint("");

    int qubits = 2;
    bool density = false;
    SINGLETON(CQuESTIniter)->Init(qubits, (int)state, 0, density);

    CmdExecutor handle;

    std::vector<std::string> initmatrix;
    handle.getAllState(initmatrix);
    osprint << "test RZZ inverse state:" << to_string(state) << " 初始振幅:[";
    for (auto temp : initmatrix)
    {
        osprint << "[" << temp << "],";
    }
    osprint << "] ";

    os.str("");
    {
        Cmd cmd;
        cmd.__set_gate("Rzz");
        cmd.targets.push_back(0);
        cmd.targets.push_back(1);
        cmd.rotation.push_back(M_PI/2);
        os << "rzz (" << cmd.rotation[0] << ") q[ 0 ],q[ 1 ]";
        cmd.desc = os.str();
        cmd.inverse = false;
        handle.Rzz(cmd);

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

    os.str("");
    {
        Cmd cmd;
        cmd.__set_gate("Rzz");
        cmd.targets.push_back(0);
        cmd.targets.push_back(1);
        cmd.rotation.push_back(M_PI/2);
        os << "rzz (" << cmd.rotation[0] << ") q[ 0 ],q[ 1 ]";
        cmd.desc = os.str();
        cmd.inverse = true;
        handle.Rzz(cmd);
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

TEST_F(CTestGateRZZ, TestInv)
{
	EXPECT_TRUE(TestInv(InitState::type::Zero));
    EXPECT_TRUE(TestInv(InitState::type::Plus));
    EXPECT_TRUE(TestInv(InitState::type::Classical));
}
