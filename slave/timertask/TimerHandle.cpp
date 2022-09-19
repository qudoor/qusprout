#include <memory>
#include <string>
#include "config/Config.h"
#include "common/qulog.h"
#include "common/Singleton.h"
#include "common/TimerTask.h"
#include "TimerHandle.h"
#include "taskmanager/TaskManager.h"
#include "ResourceTimer.h"
#include "TaskTimer.h"
#include "HeartbeatTimer.h"
#include "StatisticsTimer.h"

CTimerHandle::CTimerHandle()
{

}

CTimerHandle::~CTimerHandle()
{
    
}

void CTimerHandle::init()
{
    //1.初始化定时器（资源获取定时器/任务清理定时器/心跳维持定时器）
    auto restimer = std::make_shared<CResourceTimer>(SINGLETON(CConfig)->m_cleanResourceInterval);
    restimer->init();
    auto tasktimer = std::make_shared<CTaskTimer>(SINGLETON(CConfig)->m_cleanTaskInterval);
    tasktimer->init();
    auto hearttimer = std::make_shared<CHeartbeatTimer>(SINGLETON(CConfig)->m_cleanHeartbeatInterval);
    hearttimer->init();
    auto statisticstimer = std::make_shared<CStatisticsTimer>(SINGLETON(CConfig)->m_statisticsInterval);
    statisticstimer->init();
    SINGLETON(CTimer)->start();
    SINGLETON(CTimerManager)->addTimer(restimer);
    SINGLETON(CTimerManager)->addTimer(tasktimer);
    SINGLETON(CTimerManager)->addTimer(hearttimer);
    SINGLETON(CTimerManager)->addTimer(statisticstimer);
}

void CTimerHandle::stop()
{
    //停止定时器
    LOG(INFO) << "begin CTimer is exited.";
    SINGLETON(CTimer)->stop();
    LOG(INFO) << "CTimer is exited.";
}
