#ifndef __H__TESTGATECR__H__
#define __H__TESTGATECR__H__

#include <string>
#include <iostream>
#include <gtest/gtest.h>
#include "interface/qusproutdata_types.h"

class CTestGateCR: public testing::Test
{
public:
	CTestGateCR();
	virtual ~CTestGateCR();

public:
	bool TestInv(const InitState::type state);
};

#endif