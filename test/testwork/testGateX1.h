#ifndef __H__TESTGATEX1__H__
#define __H__TESTGATEX1__H__

#include <string>
#include <iostream>
#include <gtest/gtest.h>
#include "interface/work_types.h"

class CTestGateX1: public testing::Test
{
public:
	CTestGateX1();
	virtual ~CTestGateX1();

public:
	bool TestInv(const InitState::type state);
};

#endif