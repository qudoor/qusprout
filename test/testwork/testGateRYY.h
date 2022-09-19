#ifndef __H__TESTGATERYY__H__
#define __H__TESTGATERYY__H__

#include <string>
#include <iostream>
#include <gtest/gtest.h>
#include "interface/work_types.h"

class CTestGateRYY: public testing::Test
{
public:
	CTestGateRYY();
	virtual ~CTestGateRYY();

public:
	bool TestInv(const InitState::type state);
};

#endif