#include <gtest/gtest.h>
#include "common/Singleton.h"
#include "common/CmdParams.h"
#include "config/Config.h"
#include "common/qulog.h"

int main(int argc, char **argv)
{
    SINGLETON(CConfig)->m_verbose = 0;
    SINGLETON(CConfig)->m_logLevel = 0;
    SINGLETON(CConfig)->m_logPath = "./";
    SINGLETON(CConfig)->m_appname = "testwork";

    SINGLETON(CQuLog)->Init(SINGLETON(CConfig)->m_appname, SINGLETON(CConfig)->m_logPath, SINGLETON(CConfig)->m_logLevel);

    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
