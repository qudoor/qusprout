/****************************************************************************
*
* This code is licensed under the Apache License, Version 2.0. You may
* obtain a copy of this license in the LICENSE.txt file in the root directory
* of this source tree or at http://www.apache.org/licenses/LICENSE-2.0.
*
* Any modifications or derivative works of this code must retain this
* copyright notice, and modified files need to carry a notice indicating
* that they have been altered from the originals.
*
****************************************************************************/

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
