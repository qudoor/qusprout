#ifndef __H__TESTGATECX__H__
#define __H__TESTGATECX__H__

#include <string>
#include <iostream>
#include <gtest/gtest.h>
#include "interface/qusproutdata_types.h"

class CTestGateCX: public testing::Test
{
public:
	CTestGateCX();
	virtual ~CTestGateCX();

public:
	bool TestInv();
};

#endif