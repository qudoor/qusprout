#include <iostream>
#include "QuRootTimer.h"
#include "common/Singleton.h"
#include "resourcemanager/ResourceManager.h"
#include "register/Register.h"

CQurootTimer::CQurootTimer(const int interval)
:CTimerTask(interval)
{

}

CQurootTimer::~CQurootTimer()
{
    
}

void CQurootTimer::start()
{
    SINGLETON(CRegister)->heart();
}

void CQurootTimer::stop()
{
    SINGLETON(CRegister)->unResourceRegister();
}
