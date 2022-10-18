#ifndef __H__TESTGATEZ1__H__
#define __H__TESTGATEZ1__H__

#include <string>
#include <iostream>
#include <gtest/gtest.h>
#include "interface/qusproutdata_types.h"

class CTestGateZ1: public testing::Test
{
public:
	CTestGateZ1();
	virtual ~CTestGateZ1();

public:
	bool TestInv();
};

#endif