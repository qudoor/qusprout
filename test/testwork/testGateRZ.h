#ifndef __H__TESTGATERZ__H__
#define __H__TESTGATERZ__H__

#include <string>
#include <iostream>
#include <gtest/gtest.h>
#include "interface/work_types.h"

class CTestGateRZ: public testing::Test
{
public:
	CTestGateRZ();
	virtual ~CTestGateRZ();

public:
	bool TestInv(const InitState::type state);
};

#endif