#ifndef __H__TESTGATESDG__H__
#define __H__TESTGATESDG__H__

#include <string>
#include <iostream>
#include <gtest/gtest.h>
#include "interface/qusproutdata_types.h"

class CTestGateSdg: public testing::Test
{
public:
	CTestGateSdg();
	virtual ~CTestGateSdg();

public:
	bool TestInv();
};

#endif