#ifndef __H__TESTGATET__H__
#define __H__TESTGATET__H__

#include <string>
#include <iostream>
#include <gtest/gtest.h>
#include "interface/qusproutdata_types.h"

class CTestGateT: public testing::Test
{
public:
	CTestGateT();
	virtual ~CTestGateT();

public:
	bool TestInv(const InitState::type state);
};

#endif