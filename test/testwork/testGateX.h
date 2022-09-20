#ifndef __H__TESTGATEX__H__
#define __H__TESTGATEX__H__

#include <string>
#include <iostream>
#include <gtest/gtest.h>
#include "interface/qusproutdata_types.h"

class CTestGateX: public testing::Test
{
public:
	CTestGateX();
	virtual ~CTestGateX();

public:
	bool TestInv(const InitState::type state);
};

#endif