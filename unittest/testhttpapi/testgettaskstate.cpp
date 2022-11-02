#include "testgettaskstate.h"

CTestGetTask::CTestGetTask()
{

}

CTestGetTask::~CTestGetTask()
{

}

bool CTestGetTask::run(const int qubitnum)
{
    ASSERT_RNT(qubitnum > 0);

    m_qubitnum = qubitnum;
    bool rnt = m_handle.init(m_qubitnum);
    ASSERT_RNT(rnt);

    int taskstate = 0;
    rnt = m_handle.getTask(taskstate);
    ASSERT_RNT(rnt);

    std::cout << "result(qubitnum:" << m_qubitnum << ",taskstate:"<< taskstate << ")." << std::endl;
    
    return true;
}
