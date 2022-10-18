#ifndef __H__TESTGATEU2__H__
#define __H__TESTGATEU2__H__

#include <string>
#include <iostream>
#include <gtest/gtest.h>
#include "interface/qusproutdata_types.h"

class CTestGateU2: public testing::Test
{
public:
	CTestGateU2();
	virtual ~CTestGateU2();

public:
	bool TestInv();
};

#endif