#ifndef __H__TESTGATERZZ__H__
#define __H__TESTGATERZZ__H__

#include <string>
#include <iostream>
#include <gtest/gtest.h>
#include "interface/work_types.h"

class CTestGateRZZ: public testing::Test
{
public:
	CTestGateRZZ();
	virtual ~CTestGateRZZ();

public:
	bool TestInv(const InitState::type state);
};

#endif