#ifndef __H__TESTGATERYY__H__
#define __H__TESTGATERYY__H__

#include <string>
#include <iostream>
#include <gtest/gtest.h>
#include "interface/qusproutdata_types.h"

class CTestGateRYY: public testing::Test
{
public:
	CTestGateRYY();
	virtual ~CTestGateRYY();

public:
	bool TestInv();
};

#endif