#ifndef __H__TESTGATEP__H__
#define __H__TESTGATEP__H__

#include <string>
#include <iostream>
#include <gtest/gtest.h>
#include "interface/qusproutdata_types.h"

class CTestGateP: public testing::Test
{
public:
	CTestGateP();
	virtual ~CTestGateP();

public:
	bool TestInv();
};

#endif