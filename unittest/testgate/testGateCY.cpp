#include <sstream>
#include <iostream>
#include <fstream>
#include "testGateCY.h"
#include "common/Singleton.h"
#include "common/QuESTIniter.h"
#include "command/executor.h"

using namespace QS;

CTestGateCY::CTestGateCY()
{

}

CTestGateCY::~CTestGateCY()
{

}

/*
测试结果：
Cmd(gate=CY, targets=[1], controls=[0], rotation=[], desc=cy q[ 0 ],q[ 1 ], inverse=1)
test CY inverse state:Zero 初始振幅:[[0.707106781187, 0],[0.707106781187, 0],[0, 0],[0, 0],] 应用门后的振幅:[[0.707106781187, 0],[0, -0],[0, 0],[-0, 0.707106781187],] 逆操作后的振幅:[[0.707106781187, 0],[0.707106781187, 0],[0, 0],[0, 0],] 
Cmd(gate=CY, targets=[1], controls=[0], rotation=[], desc=cy q[ 0 ],q[ 1 ], inverse=1)
test CY inverse state:Plus 初始振幅:[[0.707106781187, 0],[0, 0],[0.707106781187, 0],[0, 0],] 应用门后的振幅:[[0.707106781187, 0],[0, -0],[0.707106781187, 0],[-0, 0],] 逆操作后的振幅:[[0.707106781187, 0],[0, 0],[0.707106781187, 0],[0, 0],] 
Cmd(gate=CY, targets=[1], controls=[0], rotation=[], desc=cy q[ 0 ],q[ 1 ], inverse=1)
test CY inverse state:Classical 初始振幅:[[0.707106781187, 0],[0.707106781187, 0],[0, 0],[0, 0],] 应用门后的振幅:[[0.707106781187, 0],[0, -0],[0, 0],[-0, 0.707106781187],] 逆操作后的振幅:[[0.707106781187, 0],[0.707106781187, 0],[0, 0],[0, 0],] 
测试结论：通过
*/
bool CTestGateCY::TestInv()
{
    std::ostringstream os("");
    std::ostringstream osprint("");

    int qubits = 2;
    bool density = false;
    SINGLETON(CQuESTIniter)->Init(qubits, density);

    CmdExecutor handle;

    {
        os.str("");
        Cmd cmd;
        cmd.__set_gate("H");
        cmd.targets.push_back(0);
        os << "h q[" << 0 << "]";
        cmd.desc = os.str();
        cmd.inverse = false;
        handle.H(cmd);
    }

    std::vector<std::string> initmatrix;
    handle.getAllState(initmatrix);
    osprint << "test CY inverse 初始振幅:[";
    for (auto temp : initmatrix)
    {
        osprint << "[" << temp << "],";
    }
    osprint << "] ";

    os.str("");
    {
        Cmd cmd;
        cmd.__set_gate("CY");
        cmd.controls.push_back(0);
        cmd.targets.push_back(1);
        os << "cy q[ 0 ],q[ 1 ]";
        cmd.desc = os.str();
        cmd.inverse = false;
        handle.CY(cmd);
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
        cmd.__set_gate("CY");
        cmd.controls.push_back(0);
        cmd.targets.push_back(1);
        os << "cy q[ 0 ],q[ 1 ]";
        cmd.desc = os.str();
        cmd.inverse = true;
        handle.CY(cmd);

        std::ostream& coutos = std::cout;
        cmd.printTo(coutos);
        coutos << std::endl;
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

TEST_F(CTestGateCY, TestInv)
{
	EXPECT_TRUE(TestInv());
}
