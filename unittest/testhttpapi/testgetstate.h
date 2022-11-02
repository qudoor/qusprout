#ifndef __H__TEST_GETSTATE__H__
#define __H__TEST_GETSTATE__H__

#include "common.h"
#include "gate.h"

class CTestGetState: public testing::Test
{
public:
    CTestGetState();
    ~CTestGetState();

    bool run(const int qubitnum = 2);

private:
    CGate m_handle;
    int m_qubitnum{2};
};

#endif
