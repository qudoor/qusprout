#include <iostream>
#include "HeartbeatTimer.h"
#include "common/Singleton.h"
#include "register/Register.h"

CHeartbeatTimer::CHeartbeatTimer(const int interval)
:CTimerTask(interval)
{

}

CHeartbeatTimer::~CHeartbeatTimer()
{
    
}

void CHeartbeatTimer::start()
{
    //定时上报心跳
    SINGLETON(CRegister)->heartbeatToMaster();
}

void CHeartbeatTimer::stop()
{

}