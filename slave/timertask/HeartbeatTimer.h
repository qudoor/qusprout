#ifndef __H__HEARTBEATTIMER__H__
#define __H__HEARTBEATTIMER__H__

#include "common/TimerTask.h"

class CHeartbeatTimer: virtual public CTimerTask
{
public:
    CHeartbeatTimer(const int interval);
    virtual ~CHeartbeatTimer();
    virtual void start();
    virtual void stop();
};

#endif
