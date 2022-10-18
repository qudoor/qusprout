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
   SINGLETON(CRegister)->heartbeatToMaster();
}

void CResourceTimer::stop()
{
   SINGLETON(CRegister)->unRegisterToMaster();
}
