#ifndef __H__TEST_GETPAULI__H__
#define __H__TEST_GETPAULI__H__

#include "common.h"
#include "gate.h"

class CTestGetEPauli: public testing::Test
{
public:
    CTestGetEPauli();
    ~CTestGetEPauli();

    bool run(const int qubitnum = 2);

private:
    CGate m_handle;
    int m_qubitnum{2};
};

#endif
