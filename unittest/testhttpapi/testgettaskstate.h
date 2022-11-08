#ifndef __H__TEST_GETTASKSTATE__H__
#define __H__TEST_GETTASKSTATE__H__

#include "common.h"
#include "gate.h"

class CTestGetTask: public testing::Test
{
public:
    CTestGetTask();
    ~CTestGetTask();

    bool run(const int qubitnum = 2);

private:
    CGate m_handle;
    int m_qubitnum{2};
};

#endif
