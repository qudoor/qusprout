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

#include <iostream>
#include "TaskTimer.h"
#include "common/Singleton.h"
#include "taskmanager/TaskManager.h"

CTaskTimer::CTaskTimer(const int interval)
:CTimerTask(interval)
{

}

CTaskTimer::~CTaskTimer()
{
    
}

void CTaskTimer::start()
{
    //定时清理任务
    SINGLETON(CTaskManager)->cleanTask();
}

void CTaskTimer::stop()
{
    SINGLETON(CTaskManager)->cleanAllTask();
}
