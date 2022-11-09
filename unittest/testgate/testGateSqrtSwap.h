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

#ifndef __H__TESTGATESQRTSWAP__H__
#define __H__TESTGATESQRTSWAP__H__

#include <string>
#include <iostream>
#include <gtest/gtest.h>
#include "interface/qusproutdata_types.h"

class CTestGateSqrtSwap: public testing::Test
{
public:
	CTestGateSqrtSwap();
	virtual ~CTestGateSqrtSwap();

public:
	bool TestInv();
};

#endif