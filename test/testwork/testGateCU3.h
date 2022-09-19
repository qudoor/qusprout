#ifndef __H__TESTGATECU3__H__
#define __H__TESTGATECU3__H__

#include <string>
#include <iostream>
#include <gtest/gtest.h>
#include "interface/work_types.h"

class CTestGateCU3: public testing::Test
{
public:
	CTestGateCU3();
	virtual ~CTestGateCU3();

public:
	bool TestInv(const InitState::type state);
};

#endif