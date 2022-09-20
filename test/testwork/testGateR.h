#ifndef __H__TESTGATER__H__
#define __H__TESTGATER__H__

#include <string>
#include <iostream>
#include <gtest/gtest.h>
#include "interface/qusproutdata_types.h"

class CTestGateR: public testing::Test
{
public:
	CTestGateR();
	virtual ~CTestGateR();

public:
	bool TestInv(const InitState::type state);
};

#endif