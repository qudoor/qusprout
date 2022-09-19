#ifndef __H__TESTGATEU__H__
#define __H__TESTGATEU__H__

#include <string>
#include <iostream>
#include <gtest/gtest.h>
#include "interface/work_types.h"

class CTestGateU: public testing::Test
{
public:
	CTestGateU();
	virtual ~CTestGateU();

public:
	bool TestInv(const InitState::type state);
};

#endif