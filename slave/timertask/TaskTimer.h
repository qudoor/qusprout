#ifndef __H__TASKTIMER__H__
#define __H__TASKTIMER__H__

#include "common/TimerTask.h"

class CTaskTimer: virtual public CTimerTask
{
public:
    CTaskTimer(const int interval);
    virtual ~CTaskTimer();
    virtual void start();
    virtual void stop();
};

#endif
