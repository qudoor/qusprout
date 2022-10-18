#ifndef __H__TESTGATEGETPAULI__H__
#define __H__TESTGATEGETPAULI__H__

#include <string>
#include <iostream>
#include <gtest/gtest.h>
#include "interface/qusproutdata_types.h"

class CTestGateGetPauli: public testing::Test
{
public:
	CTestGateGetPauli();
	virtual ~CTestGateGetPauli();

public:
	bool TestInv();
};

#endif