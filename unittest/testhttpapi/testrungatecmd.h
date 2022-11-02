#ifndef __H__TEST_RUNGATECMD__H__
#define __H__TEST_RUNGATECMD__H__

#include "common.h"
#include "gate.h"

class CTestRunGateCmd: public testing::Test
{
public:
    CTestRunGateCmd();
    ~CTestRunGateCmd();

    bool run(const int qubitnum = 2);

private:
    CGate m_handle;
    int m_qubitnum{2};
};

#endif
