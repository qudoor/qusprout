#ifndef __H__COMMON__H__
#define __H__COMMON__H__

#include <gtest/gtest.h>

#define ASSERT_RNT(rnt) \
if (false == (rnt)) { \
    return false; \
}

const int g_threadnum = 10;

#endif
