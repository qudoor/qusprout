#ifndef __H__TEST_GETPAULISUM__H__
#define __H__TEST_GETPAULISUM__H__

#include "common.h"
#include "gate.h"

class CTestGetEPauliSum: public testing::Test
{
public:
    CTestGetEPauliSum();
    ~CTestGetEPauliSum();

    bool run(const int qubitnum = 2);

private:
    CGate m_handle;
    int m_qubitnum{2};
};

#endif
