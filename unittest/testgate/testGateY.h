#ifndef __H__TESTGATEY__H__
#define __H__TESTGATEY__H__

#include <string>
#include <iostream>
#include <gtest/gtest.h>
#include "interface/qusproutdata_types.h"

class CTestGateY: public testing::Test
{
public:
	CTestGateY();
	virtual ~CTestGateY();

public:
	bool TestInv();
};

#endif