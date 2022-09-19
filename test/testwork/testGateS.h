#ifndef __H__TESTGATES__H__
#define __H__TESTGATES__H__

#include <string>
#include <iostream>
#include <gtest/gtest.h>
#include "interface/work_types.h"

class CTestGateS: public testing::Test
{
public:
	CTestGateS();
	virtual ~CTestGateS();

public:
	bool TestInv(const InitState::type state);
};

#endif