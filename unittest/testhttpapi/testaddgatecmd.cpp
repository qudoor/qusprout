
#include "testaddgatecmd.h"

CTestAddGateCmd::CTestAddGateCmd()
{

}

CTestAddGateCmd::~CTestAddGateCmd()
{

}

bool CTestAddGateCmd::run(const int qubitnum)
{
    ASSERT_RNT(qubitnum > 1);

    m_qubitnum = qubitnum;
    bool rnt = m_handle.init(m_qubitnum);
    ASSERT_RNT(rnt);

    std::vector<int> targets;
    targets.push_back(1);
    rnt = m_handle.xGate(targets);
    ASSERT_RNT(rnt);

    for (size_t i = 0; i < 2; ++i)
    {
        targets.clear();
        targets.push_back(i);
        rnt = m_handle.hGate(targets);
        ASSERT_RNT(rnt);
    }

    targets.clear();
    std::vector<int> controls;
    controls.push_back(0);
    targets.push_back(1);
    rnt = m_handle.cnotGate(controls, targets);
    ASSERT_RNT(rnt);

    targets.clear();
    targets.push_back(0);
    rnt = m_handle.hGate(targets);
    ASSERT_RNT(rnt);

    targets.clear();
    targets.push_back(0);
    rnt = m_handle.measureGate(targets);
    ASSERT_RNT(rnt);
    
    targets.clear();
    targets.push_back(0);
    GetMeasureResp getresp;
    rnt = m_handle.getMeasure(targets, getresp);
    ASSERT_RNT(rnt);

    for (auto measure : getresp.measures)
    {
        std::cout << "result(qubitnum:" << m_qubitnum << ",target:"<< measure.target << ",value:" << measure.value << ")." << std::endl;
    }
    
    return true;
}
