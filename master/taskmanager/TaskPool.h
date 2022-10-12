#ifndef __H__TASKPOOL__H__
#define __H__TASKPOOL__H__

#include <memory>
#include <hv/hthreadpool.h>

class CTaskPool
{
public:
    CTaskPool();
    ~CTaskPool();

    int init(const int minthreads, const int maxthreads, const int maxidletime);
    void addAddCmdTask(const std::string& id);
    void addRunCmdTask(const std::string& id, const int shots);
    void stop();


private:
    //最小线程
    int m_minThreads{1};

    //最大线程
    int m_maxThreads{1};

    //任务最大超时时间
    int m_maxIdleTime{0};

    std::shared_ptr<HThreadPool> m_pool;
};

#endif
