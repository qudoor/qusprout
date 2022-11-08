#ifndef __H__TEST_GETPROB__H__
#define __H__TEST_GETPROB__H__

#include "common.h"
#include "gate.h"

class CTestGetProb: public testing::Test
{
public:
    CTestGetProb();
    ~CTestGetProb();

    bool run(const int qubitnum = 2);

private:
    CGate m_handle;
    int m_qubitnum{2};
};

#endif
