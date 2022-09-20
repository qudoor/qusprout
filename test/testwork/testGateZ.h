#ifndef __H__TESTGATEZ__H__
#define __H__TESTGATEZ__H__

#include <string>
#include <iostream>
#include <gtest/gtest.h>
#include "interface/qusproutdata_types.h"

class CTestGateZ: public testing::Test
{
public:
	CTestGateZ();
	virtual ~CTestGateZ();

public:
	bool TestInv(const InitState::type state);
};

#endif