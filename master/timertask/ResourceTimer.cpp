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
   SINGLETON(CResourceManager)->cleanResource();
}

void CResourceTimer::stop()
{
   //SINGLETON(CResourceManager)->cleanAllResource();
}
