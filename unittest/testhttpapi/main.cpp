#include <gtest/gtest.h>
#include "testaddgatecmd.h"
#include "testgetamp.h"
#include "testgetepauli.h"
#include "testgetepaulisum.h"
#include "testgetmeasure.h"
#include "testgetmetrics.h"
#include "testgetstate.h"
#include "testgettaskstate.h"
#include "testinitenv.h"
#include "testmpi.h"
#include "testprob.h"
#include "testreleaseenv.h"
#include "testrungatecmd.h"
/*
TEST_F(CTestAddGateCmd, run)
{
	EXPECT_TRUE(run());
}

TEST_F(CTestGetAmp, run)
{
	EXPECT_TRUE(run());
}

TEST_F(CTestGetEPauli, run)
{
	EXPECT_TRUE(run());
}

TEST_F(CTestGetEPauliSum, run)
{
	EXPECT_TRUE(run());
}

TEST_F(CTestGetMeasure, run)
{
	EXPECT_TRUE(run());
}

TEST_F(CTestGetMetrics, run)
{
	EXPECT_TRUE(run());
}

TEST_F(CTestGetState, run)
{
	EXPECT_TRUE(run());
}

TEST_F(CTestGetTask, run)
{
	EXPECT_TRUE(run());
}

TEST_F(CTestInitEnv, run)
{
	EXPECT_TRUE(run());
}
*/
TEST_F(CTestMpi, run)
{
	EXPECT_TRUE(run());
}
/*
TEST_F(CTestGetProb, run)
{
	EXPECT_TRUE(run());
}

TEST_F(CTestReleaseEnv, run)
{
	EXPECT_TRUE(run());
}

TEST_F(CTestRunGateCmd, run)
{
	EXPECT_TRUE(run());
}
*/
int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
