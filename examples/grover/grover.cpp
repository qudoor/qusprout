#include <math.h>
#include <stdlib.h>
#include <iostream>
#include <iomanip>
#include "common/gate.h"

#define ASSERT_RNT(rnt) \
if (false == (rnt)) { \
    return false; \
}

class CGrover
{
public:
    CGrover();
    ~CGrover();

    bool init(const int qubitnum = 10);
    bool flip(const int operindexs);
    bool image();

    bool run();

private:
    CGate m_handle;
    int m_qubitnum{10};
};

CGrover::CGrover()
{

}

CGrover::~CGrover()
{

}

bool CGrover::init(const int qubitnum)
{
    ASSERT_RNT(qubitnum > 0);

    m_qubitnum = qubitnum;
    bool rnt = m_handle.init(m_qubitnum);
    ASSERT_RNT(rnt);

    rnt = m_handle.createPlusState();
    ASSERT_RNT(rnt);

    return true;
}

bool CGrover::flip(const int operindexs)
{
    std::vector<int> targets;
    std::vector<int> mczcontrols;
    std::vector<int> mcztargets;
    for (int i = 0; i < m_qubitnum; i++)
    {
        if (((operindexs >> i) & 1) == 0)
        {
            targets.push_back(i);
        }    

        if (i == m_qubitnum - 1)
        {
            mcztargets.push_back(i);
        }   
        else
        {
            mczcontrols.push_back(i);
        }
    }
    bool rnt = false;
    if (targets.size() > 0)
    {
        rnt = m_handle.xGate(targets);
        ASSERT_RNT(rnt);
    }

    rnt = m_handle.mczGate(mczcontrols, mcztargets);
    ASSERT_RNT(rnt);
    
    if (targets.size() > 0)
    {
        rnt = m_handle.xGate(targets);
        ASSERT_RNT(rnt);
    }

    return true;
}

bool CGrover::image()
{
    std::vector<int> targets;
    std::vector<int> mczcontrols;
    std::vector<int> mcztargets;
    for (int i = 0; i < m_qubitnum; i++)
    {
        targets.push_back(i); 

        if (i == m_qubitnum - 1)
        {
            mcztargets.push_back(i);
        }   
        else
        {
            mczcontrols.push_back(i);
        }
    }

    bool rnt = m_handle.hGate(targets);
    ASSERT_RNT(rnt);

    rnt = m_handle.xGate(targets);
    ASSERT_RNT(rnt);

    rnt = m_handle.mczGate(mczcontrols, mcztargets);
    ASSERT_RNT(rnt);
    
    rnt = m_handle.xGate(targets);
    ASSERT_RNT(rnt);

    rnt = m_handle.hGate(targets);
    ASSERT_RNT(rnt);

    return true;
}

bool CGrover::run()
{
    int qubitindexs = (int)(1 << m_qubitnum);
    int count = ceil(M_PI/4 * sqrt(qubitindexs));
    std::cout << "init(qubitnum:" << m_qubitnum << ",qubitindexs:"<< qubitindexs << ",count:" << count << ")." << std::endl;

    srand(time(NULL));
    int operindexs = rand() % qubitindexs;

    bool rnt = false;
    for (int i = 0; i < count; i++) {
        rnt = flip(operindexs);
        ASSERT_RNT(rnt);

        rnt = image();
        ASSERT_RNT(rnt);
        
        double amp = 0.0;
        rnt = m_handle.getProbAmp(operindexs, amp);
        ASSERT_RNT(rnt);

        std::cout << "prob of state |"<< operindexs << "> = " << std::setprecision(14) << amp << std::endl;
    }
    
    return true;
}

int main(int argc, char** argv)
{
    CGrover handle;
    bool rnt = handle.init();
    if (false == rnt)
    { 
        return -1; 
    }

    rnt = handle.run();
    if (false == rnt)
    { 
        return -1; 
    }

    return 0;
}
