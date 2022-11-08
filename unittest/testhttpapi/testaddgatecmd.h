#ifndef __H__TEST_ADDGATECMD__H__
#define __H__TEST_ADDGATECMD__H__

#include "common.h"
#include "gate.h"

class CTestAddGateCmd: public testing::Test
{
public:
    CTestAddGateCmd();
    ~CTestAddGateCmd();

    bool run(const int qubitnum = 2);

private:
    CGate m_handle;
    int m_qubitnum{2};
};

#endif
