#ifndef __H__TESTGATECRX__H__
#define __H__TESTGATECRX__H__

#include <string>
#include <iostream>
#include <gtest/gtest.h>
#include "interface/qusproutdata_types.h"

class CTestGateCRX: public testing::Test
{
public:
	CTestGateCRX();
	virtual ~CTestGateCRX();

public:
	bool TestInv();
};

#endif