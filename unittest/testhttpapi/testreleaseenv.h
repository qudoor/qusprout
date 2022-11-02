#ifndef __H__TEST_RELEASEENV__H__
#define __H__TEST_RELEASEENV__H__

#include "common.h"
#include "gate.h"

class CTestReleaseEnv: public testing::Test
{
public:
    CTestReleaseEnv();
    ~CTestReleaseEnv();

    bool run(const int qubitnum = 2);

private:
    CGate m_handle;
    int m_qubitnum{2};
};

#endif
