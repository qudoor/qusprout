#ifndef __H__TESTGATECNOT__H__
#define __H__TESTGATECNOT__H__

#include <string>
#include <iostream>
#include <gtest/gtest.h>
#include "interface/work_types.h"

class CTestGateCNOT: public testing::Test
{
public:
	CTestGateCNOT();
	virtual ~CTestGateCNOT();

public:
	bool TestInv(const InitState::type state);
};

#endif