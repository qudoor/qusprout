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

#ifndef __H__TESTGATEGETPAULI__H__
#define __H__TESTGATEGETPAULI__H__

#include <string>
#include <iostream>
#include <gtest/gtest.h>
#include "interface/qusproutdata_types.h"

class CTestGateGetPauli: public testing::Test
{
public:
	CTestGateGetPauli();
	virtual ~CTestGateGetPauli();

public:
	bool TestInv();
};

#endif