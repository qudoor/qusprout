#ifndef __H__TESTGATEU3__H__
#define __H__TESTGATEU3__H__

#include <string>
#include <iostream>
#include <gtest/gtest.h>
#include "interface/qusproutdata_types.h"

class CTestGateU3: public testing::Test
{
public:
	CTestGateU3();
	virtual ~CTestGateU3();

public:
	bool TestInv(const InitState::type state);
	bool VerifyXAndU3(const InitState::type state);
	bool VerifyRXAndU3(const InitState::type state);
};

#endif