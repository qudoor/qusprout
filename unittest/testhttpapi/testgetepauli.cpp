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

#include <stdlib.h>
#include <time.h>
#include "testgetepauli.h"

CTestGetEPauli::CTestGetEPauli()
{

}

CTestGetEPauli::~CTestGetEPauli()
{

}

bool CTestGetEPauli::run(const int qubitnum)
{
    ASSERT_RNT(qubitnum > 0);

    m_qubitnum = qubitnum;
    bool rnt = m_handle.init(m_qubitnum);
    ASSERT_RNT(rnt);

    double expect = 0;
    time_t now = time(NULL);

    srand(now);
    std::vector<PauliInfo> paulis;
    for (auto i = 0; i < m_qubitnum; ++i)
    {
        PauliInfo temp;
        temp.target = i;
        temp.opertype = rand() % 4;
        paulis.push_back(temp);
    }

    rnt = m_handle.getEPauli(paulis, expect);
    ASSERT_RNT(rnt);

    rnt = m_handle.endAdd();
    ASSERT_RNT(rnt);
    
    std::cout << "result(qubitnum:" << m_qubitnum << ",expect:"<< expect << ")." << std::endl;
    
    return true;
}
