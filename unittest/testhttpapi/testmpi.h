#ifndef __H__TEST_MPI__H__
#define __H__TEST_MPI__H__

#include "common.h"
#include "gate.h"

class CTestMpi: public testing::Test
{
public:
    CTestMpi();
    ~CTestMpi();

    bool run(const int qubitnum = 2);

private:
    CGate m_handle;
    int m_qubitnum{2};
};

#endif
