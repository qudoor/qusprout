#ifndef __H__TESTGATECY__H__
#define __H__TESTGATECY__H__

#include <string>
#include <iostream>
#include <gtest/gtest.h>
#include "interface/qusproutdata_types.h"

class CTestGateCY: public testing::Test
{
public:
	CTestGateCY();
	virtual ~CTestGateCY();

public:
	bool TestInv(const InitState::type state);
};

#endif