#ifndef __H__TEST_GETMETRICS__H__
#define __H__TEST_GETMETRICS__H__

#include "common.h"
#include "gate.h"

class CTestGetMetrics: public testing::Test
{
public:
    CTestGetMetrics();
    ~CTestGetMetrics();

    bool run(const int qubitnum = 1);

private:
    CGate m_handle;
    int m_qubitnum{2};
};

#endif
