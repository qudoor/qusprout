#include <iostream>
#include "ResourceTimer.h"
#include "common/Singleton.h"
#include "register/Register.h"

CResourceTimer::CResourceTimer(const int interval)
:CTimerTask(interval)
{

}

CResourceTimer::~CResourceTimer()
{
    
}

void CResourceTimer::start()
{
   //定时上报资源
   SINGLETON(CRegister)->reportResourceToMaster(); 
}

void CResourceTimer::stop()
{

}
