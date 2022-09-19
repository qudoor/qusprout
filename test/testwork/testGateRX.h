#ifndef __H__TESTGATERX__H__
#define __H__TESTGATERX__H__

#include <string>
#include <iostream>
#include <gtest/gtest.h>
#include "interface/work_types.h"

class CTestGateRX: public testing::Test
{
public:
	CTestGateRX();
	virtual ~CTestGateRX();

public:
	bool TestInv(const InitState::type state);
};

#endif