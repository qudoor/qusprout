#include "testgetmetrics.h"

CTestGetMetrics::CTestGetMetrics()
{

}

CTestGetMetrics::~CTestGetMetrics()
{

}

bool CTestGetMetrics::run(const int qubitnum)
{
    ASSERT_RNT(qubitnum > 0);

    m_qubitnum = qubitnum;
    bool rnt = m_handle.init(m_qubitnum);
    ASSERT_RNT(rnt);

    std::string metrics = "";
    rnt = m_handle.getMetrics(metrics);
    ASSERT_RNT(rnt);
    std::cout << "result(qubitnum:" << m_qubitnum << ",metrics:"<< metrics << ")." << std::endl;
    
    rnt = m_handle.endAdd();
    ASSERT_RNT(rnt);
    
    return true;
}
