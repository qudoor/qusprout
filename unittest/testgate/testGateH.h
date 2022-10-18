#ifndef __H__TESTGATEH__H__
#define __H__TESTGATEH__H__

#include <string>
#include <iostream>
#include <gtest/gtest.h>
#include "interface/qusproutdata_types.h"

class CTestGateH: public testing::Test
{
public:
	CTestGateH();
	virtual ~CTestGateH();

public:
	bool TestInv();
};

#endif