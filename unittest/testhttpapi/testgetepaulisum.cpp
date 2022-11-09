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
#include "testgetepaulisum.h"

CTestGetEPauliSum::CTestGetEPauliSum()
{

}

CTestGetEPauliSum::~CTestGetEPauliSum()
{

}

bool CTestGetEPauliSum::run(const int qubitnum)
{
    ASSERT_RNT(qubitnum > 0);

    m_qubitnum = qubitnum;
    bool rnt = m_handle.init(m_qubitnum);
    ASSERT_RNT(rnt);

    double expect = 0;
    time_t now = time(NULL);

    const int termsnum = 2;
    std::vector<int> opertypes; 
    std::vector<double> terms;
    for (auto i = 0; i < termsnum; ++i)
    {
        srand(now + i);
        double coeff = rand() % 1000;
        coeff += coeff/100.0;
        if (rand() % 2 == 0)
        {
            coeff = -coeff;
        }
        terms.push_back(coeff);
    }
    for (auto i = 0; i < termsnum*qubitnum; ++i)
    {
        srand(now + i);
        opertypes.push_back(rand() % 4);
    }

    rnt = m_handle.getEPauliSum(opertypes, terms, expect);
    ASSERT_RNT(rnt);

    rnt = m_handle.endAdd();
    ASSERT_RNT(rnt);
    
    std::cout << "result(qubitnum:" << m_qubitnum << ",expect:"<< expect << ")." << std::endl;
    
    return true;
}
