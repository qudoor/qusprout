#ifndef __H__TESTGATECRY__H__
#define __H__TESTGATECRY__H__

#include <string>
#include <iostream>
#include <gtest/gtest.h>
#include "interface/work_types.h"

class CTestGateCRY: public testing::Test
{
public:
	CTestGateCRY();
	virtual ~CTestGateCRY();

public:
	bool TestInv(const InitState::type state);
};

#endif