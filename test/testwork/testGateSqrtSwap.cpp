#include <sstream>
#include <iostream>
#include <fstream>
#include "testGateSqrtSwap.h"
#include "common/Singleton.h"
#include "common/QuESTIniter.h"
#include "command/executor.h"

using namespace QS;

CTestGateSqrtSwap::CTestGateSqrtSwap()
{

}

CTestGateSqrtSwap::~CTestGateSqrtSwap()
{

}

/*
测试结果：
Cmd(gate=SqrtSwap, targets=[0, 1], controls=[], rotation=[], desc=sqrtswap q[ 0 ],q[ 1 ], inverse=0)
test SqrtSwap inverse state:Zero 初始振幅:[[1, 0],[0, 0],[0, 0],[0, 0],] 应用门后的振幅:[[1, 0],[0, 0],[0, 0],[0, 0],] 逆操作后的振幅:[[1, 0],[0, 0],[0, 0],[0, 0],] 
Cmd(gate=SqrtSwap, targets=[0, 1], controls=[], rotation=[], desc=sqrtswap q[ 0 ],q[ 1 ], inverse=0)
test SqrtSwap inverse state:Plus 初始振幅:[[0.5, 0],[0.5, 0],[0.5, 0],[0.5, 0],] 应用门后的振幅:[[0.5, 0],[0.5, 0],[0.5, 0],[0.5, 0],] 逆操作后的振幅:[[0.5, 0],[0.5, 0],[0.5, 0],[0.5, 0],] 
Cmd(gate=SqrtSwap, targets=[0, 1], controls=[], rotation=[], desc=sqrtswap q[ 0 ],q[ 1 ], inverse=0)
test SqrtSwap inverse state:Classical 初始振幅:[[1, 0],[0, 0],[0, 0],[0, 0],] 应用门后的振幅:[[1, 0],[0, 0],[0, 0],[0, 0],] 逆操作后的振幅:[[1, 0],[0, 0],[0, 0],[0, 0],] 
测试结论：不确定
*/
bool CTestGateSqrtSwap::TestInv(const InitState::type state)
{
    std::ostringstream os("");
    std::ostringstream osprint("");

    int qubits = 2;
    bool density = false;
    SINGLETON(CQuESTIniter)->Init(qubits, (int)state, 0, density);

    CmdExecutor handle;

    std::vector<std::string> initmatrix;
    handle.getAllState(initmatrix);
    osprint << "test SqrtSwap inverse state:" << to_string(state) << " 初始振幅:[";
    for (auto temp : initmatrix)
    {
        osprint << "[" << temp << "],";
    }
    osprint << "] ";

    os.str("");
    {
        Cmd cmd;
        cmd.__set_gate("SqrtSwap");
        cmd.targets.push_back(0);
        cmd.targets.push_back(1);
        os << "sqrtswap q[ 0 ],q[ 1 ]";
        cmd.desc = os.str();
        cmd.inverse = false;
        handle.SqrtSwap(cmd);

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
        cmd.__set_gate("SqrtSwap");
        cmd.targets.push_back(0);
        cmd.targets.push_back(1);
        os << "sqrtswap q[ 0 ],q[ 1 ]";
        cmd.desc = os.str();
        cmd.inverse = true;
        handle.SqrtSwap(cmd);
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

TEST_F(CTestGateSqrtSwap, TestInv)
{
	EXPECT_TRUE(TestInv(InitState::type::Zero));
    EXPECT_TRUE(TestInv(InitState::type::Plus));
    EXPECT_TRUE(TestInv(InitState::type::Classical));
}