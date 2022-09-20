#ifndef __H__TESTGATECU__H__
#define __H__TESTGATECU__H__

#include <string>
#include <iostream>
#include <gtest/gtest.h>
#include "interface/qusproutdata_types.h"

class CTestGateCU: public testing::Test
{
public:
	CTestGateCU();
	virtual ~CTestGateCU();

public:
	bool TestInv(const InitState::type state);
};

#endif