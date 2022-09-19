#ifndef __H__TESTGATECRZ__H__
#define __H__TESTGATECRZ__H__

#include <string>
#include <iostream>
#include <gtest/gtest.h>
#include "interface/work_types.h"

class CTestGateCRZ: public testing::Test
{
public:
	CTestGateCRZ();
	virtual ~CTestGateCRZ();

public:
	bool TestInv(const InitState::type state);
};

#endif