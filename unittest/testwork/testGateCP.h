#ifndef __H__TESTGATECP__H__
#define __H__TESTGATECP__H__

#include <string>
#include <iostream>
#include <gtest/gtest.h>
#include "interface/qusproutdata_types.h"

class CTestGateCP: public testing::Test
{
public:
	CTestGateCP();
	virtual ~CTestGateCP();

public:
	bool TestInv(const InitState::type state);
};

#endif