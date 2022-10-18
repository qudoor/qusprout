#ifndef __H__TESTALL__H__
#define __H__TESTALL__H__

#include <string>
#include <iostream>
#include <gtest/gtest.h>
#include "interface/qusproutdata_types.h"

class CTestAll: public testing::Test
{
public:
	CTestAll();
	virtual ~CTestAll();

public:
	bool TestInv();
};

#endif