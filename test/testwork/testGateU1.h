#ifndef __H__TESTGATEU1__H__
#define __H__TESTGATEU1__H__

#include <string>
#include <iostream>
#include <gtest/gtest.h>
#include "interface/work_types.h"

class CTestGateU1: public testing::Test
{
public:
	CTestGateU1();
	virtual ~CTestGateU1();

public:
	bool TestInv(const InitState::type state);
};

#endif