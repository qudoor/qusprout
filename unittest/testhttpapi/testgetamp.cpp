/****************************************************************************
*
* This code is licensed under the Apache License, Version 2.0. You may
* obtain a copy of this license in the LICENSE.txt file in the root directory
* of this source tree or at http://www.apache.org/licenses/LICENSE-2.0.
*
* Any modifications or derivative works of this code must retain this
* copyright notice, and modified files need to carry a notice indicating
* that they have been altered from the originals.
*
****************************************************************************/

#include <math.h>
#include "testgetamp.h"

CTestGetAmp::CTestGetAmp()
{

}

CTestGetAmp::~CTestGetAmp()
{

}

bool CTestGetAmp::flip(const int operindexs)
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

bool CTestGetAmp::image()
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

bool CTestGetAmp::run(const int qubitnum)
{
    ASSERT_RNT(qubitnum > 0);
    
    m_qubitnum = qubitnum;
    bool rnt = m_handle.init(m_qubitnum);
    ASSERT_RNT(rnt);

    rnt = m_handle.createPlusState();
    ASSERT_RNT(rnt);

    int qubitindexs = (int)(1 << m_qubitnum);
    int count = ceil(M_PI/4 * sqrt(qubitindexs));
    std::cout << "init(qubitnum:" << m_qubitnum << ",qubitindexs:"<< qubitindexs << ",count:" << count << ")." << std::endl;

    srand(time(NULL));
    int operindexs = rand() % qubitindexs;

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

    rnt = m_handle.endAdd();
    ASSERT_RNT(rnt);
        
    return true;
}
