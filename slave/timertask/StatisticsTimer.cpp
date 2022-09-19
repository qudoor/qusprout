#include <iostream>
#include "StatisticsTimer.h"
#include "common/Singleton.h"
#include "register/Register.h"

CStatisticsTimer::CStatisticsTimer(const int interval)
:CTimerTask(interval)
{

}

CStatisticsTimer::~CStatisticsTimer()
{
    
}

void CStatisticsTimer::start()
{
   //上报统计信息
   SINGLETON(CRegister)->ReportStatisticsInfo(); 
}

void CStatisticsTimer::stop()
{

}
