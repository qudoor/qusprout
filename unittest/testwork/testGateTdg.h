#ifndef __H__TESTGATETDG__H__
#define __H__TESTGATETDG__H__

#include <string>
#include <iostream>
#include <gtest/gtest.h>
#include "interface/qusproutdata_types.h"

class CTestGateTdg: public testing::Test
{
public:
	CTestGateTdg();
	virtual ~CTestGateTdg();

public:
	bool TestInv(const InitState::type state);
};

#endif