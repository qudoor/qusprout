#ifndef __H__TESTGATERXX__H__
#define __H__TESTGATERXX__H__

#include <string>
#include <iostream>
#include <gtest/gtest.h>
#include "interface/qusproutdata_types.h"

class CTestGateRXX: public testing::Test
{
public:
	CTestGateRXX();
	virtual ~CTestGateRXX();

public:
	bool TestInv(const InitState::type state);
};

#endif