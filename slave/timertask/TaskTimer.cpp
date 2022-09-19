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
    SINGLETON(CTaskManager)->timerCleanTask();
}

void CTaskTimer::stop()
{

}