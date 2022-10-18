#ifndef __H__TESTGATERY__H__
#define __H__TESTGATERY__H__

#include <string>
#include <iostream>
#include <gtest/gtest.h>
#include "interface/qusproutdata_types.h"

class CTestGateRY: public testing::Test
{
public:
	CTestGateRY();
	virtual ~CTestGateRY();

public:
	bool TestInv();
};

#endif