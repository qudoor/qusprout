#ifndef __H__TESTGATEISWAP__H__
#define __H__TESTGATEISWAP__H__

#include <string>
#include <iostream>
#include <gtest/gtest.h>
#include "interface/qusproutdata_types.h"

class CTestGateiSwap: public testing::Test
{
public:
	CTestGateiSwap();
	virtual ~CTestGateiSwap();

public:
	bool TestInv();
};

#endif