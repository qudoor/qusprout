#ifndef __H__TESTGATECRY__H__
#define __H__TESTGATECRY__H__

#include <string>
#include <iostream>
#include <gtest/gtest.h>
#include "interface/qusproutdata_types.h"

class CTestGateCRY: public testing::Test
{
public:
	CTestGateCRY();
	virtual ~CTestGateCRY();

public:
	bool TestInv();
};

#endif