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
#include "register/Register.h"

CTimerHandle::CTimerHandle()
{

}

CTimerHandle::~CTimerHandle()
{
    
}

void CTimerHandle::init()
{
    SINGLETON(CRegister)->registerToMaster();
    auto restimer = std::make_shared<CResourceTimer>(SINGLETON(CConfig)->m_masterHeartInterval);
    restimer->init();
    auto tasktimer = std::make_shared<CTaskTimer>(SINGLETON(CConfig)->m_cleanTaskInterval);
    tasktimer->init();
    SINGLETON(CTimer)->start();
    SINGLETON(CTimerManager)->addTimer(restimer);
    SINGLETON(CTimerManager)->addTimer(tasktimer);
}

void CTimerHandle::stop()
{
    //停止定时器
    LOG(INFO) << "begin CTimer is exited.";
    SINGLETON(CTimer)->stop();
    LOG(INFO) << "CTimer is exited.";
}
