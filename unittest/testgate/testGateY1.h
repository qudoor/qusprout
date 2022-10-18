#ifndef __H__TESTGATEY1__H__
#define __H__TESTGATEY1__H__

#include <string>
#include <iostream>
#include <gtest/gtest.h>
#include "interface/qusproutdata_types.h"

class CTestGateY1: public testing::Test
{
public:
	CTestGateY1();
	virtual ~CTestGateY1();

public:
	bool TestInv();
};

#endif