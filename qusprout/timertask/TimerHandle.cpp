#include <memory>
#include <string>
#include "config/Config.h"
#include "common/qulog.h"
#include "common/Singleton.h"
#include "common/TimerTask.h"
#include "TimerHandle.h"
#include "taskmanager/TaskManager.h"
#include "TaskTimer.h"
#include "QuRootTimer.h"

CTimerHandle::CTimerHandle()
{

}

CTimerHandle::~CTimerHandle()
{
    
}

void CTimerHandle::init()
{
    //1.初始化定时器（任务清理定时器）
    auto tasktimer = std::make_shared<CTaskTimer>(SINGLETON(CConfig)->m_cleanTaskInterval);
    tasktimer->init();
    SINGLETON(CTimer)->start();
    SINGLETON(CTimerManager)->addTimer(tasktimer);
    if (1 == SINGLETON(CConfig)->m_qurootEnable)
    {
        auto quroottimer = std::make_shared<CQurootTimer>(SINGLETON(CConfig)->m_qurootHeartInterval);
        quroottimer->init();
        SINGLETON(CTimerManager)->addTimer(quroottimer);
    }
}

void CTimerHandle::stop()
{
    //停止定时器
    SINGLETON(CTimer)->stop();
    LOG(INFO) << "CTimer is exited.";
}
