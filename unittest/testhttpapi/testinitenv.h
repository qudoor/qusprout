#ifndef __H__TEST_INITENV__H__
#define __H__TEST_INITENV__H__

#include "common.h"
#include "gate.h"

class CTestInitEnv: public testing::Test
{
public:
    CTestInitEnv();
    ~CTestInitEnv();

    bool run(const int qubitnum = 2);

private:
    CGate m_handle;
    int m_qubitnum{2};
};

#endif
