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
	bool TestInv(const InitState::type state);
};

#endif