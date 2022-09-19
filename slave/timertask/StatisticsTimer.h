#ifndef __H__STATISTICSTIMER__H__
#define __H__STATISTICSTIMER__H__

#include "common/TimerTask.h"

class CStatisticsTimer: virtual public CTimerTask
{
public:
    CStatisticsTimer(const int interval);
    virtual ~CStatisticsTimer();
    virtual void start();
    virtual void stop();
};

#endif
