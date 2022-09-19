#ifndef __H__RESOURCETIMER__H__
#define __H__RESOURCETIMER__H__

#include "common/TimerTask.h"

class CResourceTimer: virtual public CTimerTask
{
public:
    CResourceTimer(const int interval);
    virtual ~CResourceTimer();
    virtual void start();
    virtual void stop();
};

#endif
