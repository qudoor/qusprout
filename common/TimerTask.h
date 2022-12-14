/****************************************************************************
*
* This code is licensed under the Apache License, Version 2.0. You may
* obtain a copy of this license in the LICENSE.txt file in the root directory
* of this source tree or at http://www.apache.org/licenses/LICENSE-2.0.
*
* Any modifications or derivative works of this code must retain this
* copyright notice, and modified files need to carry a notice indicating
* that they have been altered from the originals.
*
****************************************************************************/

#ifndef TIMERTASK_H
#define TIMERTASK_H

#include <map>
#include <string>
#include <mutex>
#include <memory>
#include <condition_variable>

template <class T>
struct myless: std::binary_function <T, T, bool>
{
	bool operator() (const T& x, const T& y) const
	{
		return x < y;
	}
};

//定时任务虚函数类
class CTimerTask 
{
public:
    CTimerTask(const int interval);
    virtual ~CTimerTask();

    void init();

    virtual void start() = 0;
    virtual void stop() = 0;

    std::string getTaskId() const;
    int getStartTime() const;
    void setNextTime();

private:
    int m_interval{0};
    int m_startTime{0};
    std::string m_taskId{""};
};

//定时任务管理类
class CTimerManager 
{
public:
    CTimerManager();
    ~CTimerManager();

    void startTimers();
    void stopTimers();
    void addTimer(const std::shared_ptr<CTimerTask> task);
    void removeTimer(const std::shared_ptr<CTimerTask> task);

private:
    bool m_isStop{true};
    std::mutex m_mutex;
    std::condition_variable m_waitHandle;
    std::multimap<int, std::shared_ptr<CTimerTask>, myless<int>>  m_mapTaskList;
};

//定时器
class CTimer
{
public:
    CTimer();
    ~CTimer();

    void start();
    void stop();
};

#endif