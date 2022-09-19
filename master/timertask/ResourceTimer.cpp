#include <iostream>
#include "ResourceTimer.h"
#include "common/Singleton.h"
#include "resourcemanager/ResourceManager.h"

CResourceTimer::CResourceTimer(const int interval)
:CTimerTask(interval)
{

}

CResourceTimer::~CResourceTimer()
{
    
}

void CResourceTimer::start()
{
   //定时清理机器资源和清理机器的所有任务
   SINGLETON(CResourceManager)->timerCleanResource();
}

void CResourceTimer::stop()
{

}
