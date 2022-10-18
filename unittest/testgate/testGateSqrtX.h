#ifndef __H__TESTGATESQRTX__H__
#define __H__TESTGATESQRTX__H__

#include <string>
#include <iostream>
#include <gtest/gtest.h>
#include "interface/qusproutdata_types.h"

class CTestGateSqrtX: public testing::Test
{
public:
	CTestGateSqrtX();
	virtual ~CTestGateSqrtX();

public:
	bool TestInv();
};

#endif