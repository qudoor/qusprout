#include "TaskPool.h"
#include "TaskManager.h"
#include "common/qulog.h"
#include "common/Singleton.h"
#include "comm/errorcode.h"

void runAddCmdTask(const std::string& id)
{
    SINGLETON(CTaskManager)->runAddCmdTask(id);
}

void runRunCmdTask(const std::string& id, const int shots)
{
    SINGLETON(CTaskManager)->runRunCmdTask(id, shots);
}

CTaskPool::CTaskPool()
{

}

CTaskPool::~CTaskPool()
{
    
}

int CTaskPool::init(const int minthreads, const int maxthreads, const int maxidletime)
{
    m_minThreads = minthreads;
    m_maxThreads = maxthreads;
    m_maxIdleTime = maxidletime;

    m_pool = std::make_shared<HThreadPool>(m_minThreads, m_maxThreads, m_maxIdleTime);
    return m_pool->start();
}

void CTaskPool::addAddCmdTask(const std::string& id)
{
    m_pool->commit(runAddCmdTask, id);
}

void CTaskPool::addRunCmdTask(const std::string& id, const int shots)
{
    m_pool->commit(runRunCmdTask, id, shots);
}

void CTaskPool::stop()
{
    m_pool->wait();
    m_pool->stop();
}