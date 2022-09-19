#ifndef __H__TESTGATECU1__H__
#define __H__TESTGATECU1__H__

#include <string>
#include <iostream>
#include <gtest/gtest.h>
#include "interface/work_types.h"

class CTestGateCU1: public testing::Test
{
public:
	CTestGateCU1();
	virtual ~CTestGateCU1();

public:
	bool TestInv(const InitState::type state);
};

#endif