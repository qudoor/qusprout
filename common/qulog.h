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

#ifndef QULOG_H
#define QULOG_H

#include <string>
#include <glog/logging.h>

class CQuLog
{
public:
    CQuLog();
    virtual ~CQuLog();

    bool Init(std::string szAppName, std::string szLogPath, int nMinLogLevel);
};

#endif