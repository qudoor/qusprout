#ifndef __H__TEST_GETAMP__H__
#define __H__TEST_GETAMP__H__

#include "common.h"
#include "gate.h"

class CTestGetAmp: public testing::Test
{
public:
    CTestGetAmp();
    ~CTestGetAmp();

    bool run(const int qubitnum = 10);

private:
    bool flip(const int operindexs);
    bool image();

private:
    CGate m_handle;
    int m_qubitnum{2};
};

#endif
