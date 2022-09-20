#ifndef __H__TESTGATECZ__H__
#define __H__TESTGATECZ__H__

#include <string>
#include <iostream>
#include <gtest/gtest.h>
#include "interface/qusproutdata_types.h"

class CTestGateCZ: public testing::Test
{
public:
	CTestGateCZ();
	virtual ~CTestGateCZ();

public:
	bool TestInv(const InitState::type state);
};

#endif