#ifndef __H__TESTGATESWAP__H__
#define __H__TESTGATESWAP__H__

#include <string>
#include <iostream>
#include <gtest/gtest.h>
#include "interface/work_types.h"

class CTestGateSwap: public testing::Test
{
public:
	CTestGateSwap();
	virtual ~CTestGateSwap();

public:
	bool TestInv(const InitState::type state);
};

#endif