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

#ifndef __H__TEST_GETPAULISUM__H__
#define __H__TEST_GETPAULISUM__H__

#include "common.h"
#include "gate.h"

class CTestGetEPauliSum: public testing::Test
{
public:
    CTestGetEPauliSum();
    ~CTestGetEPauliSum();

    bool run(const int qubitnum = 2);

private:
    CGate m_handle;
    int m_qubitnum{2};
};

#endif
