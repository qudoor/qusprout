#ifndef __H__QUROOTTIMER__H__
#define __H__QUROOTTIMER__H__

#include "common/TimerTask.h"

class CQurootTimer: virtual public CTimerTask
{
public:
    CQurootTimer(const int interval);
    virtual ~CQurootTimer();
    virtual void start();
    virtual void stop();
};

#endif
