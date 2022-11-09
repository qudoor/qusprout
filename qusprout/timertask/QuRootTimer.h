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

#ifndef __H__QUROOTTIMER__H__
#define __H__QUROOTTIMER__H__

#include "common/TimerTask.h"

class CQurootTimer: virtual public CTimerTask
{
public:
    CQurootTimer(const int interval);
    virtual ~CQurootTimer();
    virtual void start();
    virtual void stop();
};

#endif
