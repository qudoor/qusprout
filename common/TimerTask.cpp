#include <sstream>
#include <iostream>
#include <ctime>
#include <thread>
#include <functional>
#include "TimerTask.h"
#include "Singleton.h"
#include "qulog.h"

CTimerTask::CTimerTask(const int interval)
:m_interval(interval)
{
   
}

CTimerTask::~CTimerTask()
{
}

void CTimerTask::init()
{
    m_startTime = time(NULL) + m_interval;
    char addr[20] = {0};
    snprintf(addr, 19, "%p", this);
    m_taskId = addr;
    LOG(INFO) << "CTimerTask init(taskid:" << m_taskId << ").";
}

int CTimerTask::getStartTime() const
{
    return m_startTime;
}

void CTimerTask::setNextTime()
{
    m_startTime = time(NULL) + m_interval;
}

std::string CTimerTask::getTaskId() const
{
    return m_taskId;
}

CTimerManager::CTimerManager()
{

}

CTimerManager::~CTimerManager()
{

}

void CTimerManager::startTimers()
{
    int curr = time(NULL);
    int waittime = 0;
    m_isStop = false;
    while(false == m_isStop)
    {
        waittime = 0;
        {
            std::lock_guard<std::mutex> guard(m_mutex);
            auto iter = m_mapTaskList.begin();
            while (iter != m_mapTaskList.end())
            {
                curr = time(NULL);
                auto task = iter->second;
                if (task->getStartTime() > curr)
                {
                    //超时的定时任务已经都处理完成
                    waittime = task->getStartTime() - curr;
                    break;
                }

                //有任务超时，执行任务
                task->start();

                //更新任务的超时时间，删除再插入为了重新排序
                iter = m_mapTaskList.erase(iter);
                task->setNextTime();
                m_mapTaskList.insert(std::pair<int, std::shared_ptr<CTimerTask>>(task->getStartTime(), task));
            }
        }
        if (0 == waittime)
        {
            waittime = 1;
        }
        //等待最小的超时时间
        std::unique_lock<std::mutex> lock(m_mutex);
        const auto status = m_waitHandle.wait_for(lock, std::chrono::milliseconds(waittime));
        if(false == m_isStop && status != std::cv_status::timeout)
        {
            std::this_thread::sleep_for(std::chrono::seconds(waittime));
        }
    }
}

void CTimerManager::stopTimers()
{
    m_isStop = true;
    std::lock_guard<std::mutex> guard(m_mutex);
    for (auto iter = m_mapTaskList.begin(); iter != m_mapTaskList.end(); iter++)
	{
		iter->second->stop();
	}
    m_waitHandle.notify_one();
}

void CTimerManager::addTimer(const std::shared_ptr<CTimerTask> task)
{
    std::lock_guard<std::mutex> guard(m_mutex);
    m_mapTaskList.insert(std::pair<int,std::shared_ptr<CTimerTask>>(task->getStartTime(), task));
    m_waitHandle.notify_one();
}

void CTimerManager::removeTimer(const std::shared_ptr<CTimerTask> task)
{
    std::lock_guard<std::mutex> guard(m_mutex);
    auto tasklist = m_mapTaskList.equal_range(task->getStartTime());
	for (auto iter = tasklist.first; iter != tasklist.second; ++iter)
	{
		if (task->getTaskId() == iter->second->getTaskId())
		{
			iter = m_mapTaskList.erase(iter);
			break;
		}
	}
}

CTimer::CTimer()
{

}

CTimer::~CTimer()
{

}

void CTimer::start()
{
    std::thread thd(std::bind(&CTimerManager::startTimers, SINGLETON(CTimerManager)));
    thd.detach();
}

void CTimer::stop()
{
    SINGLETON(CTimerManager)->stopTimers();
}