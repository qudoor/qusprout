#ifndef __H__TEST_GETMEASURE__H__
#define __H__TEST_GETMEASURE__H__

#include "common.h"
#include "gate.h"

class CTestGetMeasure: public testing::Test
{
public:
    CTestGetMeasure();
    ~CTestGetMeasure();

    bool run(const int qubitnum = 2);

private:
    CGate m_handle;
    int m_qubitnum{2};
};

#endif
