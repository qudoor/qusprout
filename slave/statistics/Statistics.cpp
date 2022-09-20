#include <sstream>
#include <time.h>
#include <algorithm>
#include "Statistics.h"
#include "common/qulog.h"
#include "common/Singleton.h"
#include "register/Register.h"
#include "taskmanager/TaskManager.h"
#include "common/SystemHandle.h"

CElapsedTime::CElapsedTime()
{
    m_start = std::chrono::steady_clock::now();
}

CElapsedTime::~CElapsedTime()
{

}

int CElapsedTime::operator()()
{
    std::chrono::time_point<std::chrono::steady_clock> end = std::chrono::steady_clock::now();
    //秒
    std::chrono::duration<double> elapsed = end - m_start;
    //毫秒
    return (int)(elapsed.count()/1000.0);
}

CStatistics::CStatistics()
{
    m_beginTime = time(NULL);
}

CStatistics::~CStatistics()
{

}

//初始化
int CStatistics::init()
{
    return 0;
}

//接口调用耗时统计/调用次数统计/总任务数统计
void CStatistics::addStatisticsTimeAndCount(const ExecCmdType::type type, const std::string& interName, const int qubitNum, const int elapsedTime)
{
    std::ostringstream os("");
   
    std::lock_guard<std::mutex> guard(m_mutex);
    int elapsed = getelapsedTimeRange(elapsedTime);
    os << type << "_" << interName << "_" << qubitNum << "_" << elapsed;
    SisInterfaceTime& value = m_interTime[os.str()];
    value.tag.base.hostname = SINGLETON(CRegister)->getHostName();
    value.tag.base.type = type;
    value.tag.interface_name = interName;
    value.tag.qubits = qubitNum;
    value.elapsed = elapsed;
    value.count += 1;

    os.str("");
    os << type << "_" << interName << "_" << qubitNum;
    SisInterfaceCount& value1 = m_interCount[os.str()];
    value1.tag.base.hostname = value.tag.base.hostname;
    value1.tag.base.type = type;
    value1.tag.interface_name = interName;
    value1.tag.qubits = qubitNum;
    value1.count += 1;

    if (interName == Tag_Iinterface_initQubits)
    {
        os.str("");
        os << type << "_" << qubitNum;
        SisAllTaskNum& value2 = m_allTaskNum[os.str()];
        value2.tag.base.hostname = value.tag.base.hostname;
        value2.tag.base.type = type;
        value2.tag.qubits = qubitNum;
        value2.num += 1;
    }
}

//接口调用返回码次数统计
void CStatistics::addStatisticsCodeCount(const ExecCmdType::type type, const std::string& interName, const int qubitNum, const int code)
{
    std::ostringstream os("");
    os << type << "_" << interName << "_" << qubitNum << "_" << code;

    std::lock_guard<std::mutex> guard(m_mutex);
    SisCodeCount& value = m_codeCount[os.str()];

    value.tag.base.hostname = SINGLETON(CRegister)->getHostName();
    value.tag.base.type = type;
    value.tag.interface_name = interName;
    value.tag.qubits = qubitNum;
    value.code = code;

    value.count += 1;
}

//获取统计信息
void CStatistics::getStatisticsInfo(StatisticsInfo& sis)
{
    MemUseInfo mem;
    int ret = SINGLETON(CSystemHandle)->getMemUseInfo(mem);
    if (ret != 0)
    {
        LOG(ERROR) << "getMemUseInfo failed(ret:" << ret << ").";
        return;
    }

    int cpunum = SINGLETON(CSystemHandle)->getCpuNum();
    if (cpunum <= 0)
    {
        LOG(ERROR) << "getCpuNum failed(ret:" << cpunum << ").";
        return;
    }

    //1.机器固定资源信息
    sis.fixed_info.cpu_num = cpunum;
    sis.fixed_info.cpu_memory = mem.totalRam;
    sis.fixed_info.gpu_type = SisGpuType::SisGpuType_Default;
    sis.fixed_info.gpu_memory = 0;

    //2.机器变化资源信息
    sis.dyna_info.cpu_used_memory = mem.totalRam - mem.freeRam;
    sis.dyna_info.gpu_used_memory = 0;

    //3.当前运行中的任务数统计
    SINGLETON(CTaskManager)->getCurrTaskInfo(sis.curr_task_num);

    {
        std::lock_guard<std::mutex> guard(m_mutex);
        //4.总任务数统计
        sis.all_task_num.insert(m_allTaskNum.begin(), m_allTaskNum.end());

        //5.接口调用次数统计
        sis.inter_cout.insert(m_interCount.begin(), m_interCount.end());

        //6.接口调用耗时统计
        sis.inter_time.insert(m_interTime.begin(), m_interTime.end());

        //7.接口调用返回码次统计
        sis.code_count.insert(m_codeCount.begin(), m_codeCount.end());

        //8.开始时间
        sis.begin_time = m_beginTime;

        //9.结束时间
        sis.end_time = time(NULL);

        //10.清零数据
        m_allTaskNum.clear();
        m_interCount.clear();
        m_interTime.clear();
        m_codeCount.clear();
        m_beginTime = sis.end_time;
    }
}

//获取耗时时间范围
int CStatistics::getelapsedTimeRange(const int elapsedTime)
{
    //时间范围，单位：毫秒
    const std::set<int> range = {100, 200, 500, 1000, 2000, 3000, 5000, 8000, 10000, 60000, 120000, 300000};
    auto iter = range.begin();
    for (; iter != range.end(); ++iter)
    {
        if (elapsedTime <= *iter)
        {
            return *iter;
        }
    }

    return (*(--iter));
}

CStatisticsTimeAndCount::CStatisticsTimeAndCount(const ExecCmdType::type type, const std::string& interName, const int qubitNum)
:m_type(type), m_interName(interName), m_qubitNum(qubitNum)
{

}

CStatisticsTimeAndCount::~CStatisticsTimeAndCount()
{
    SINGLETON(CStatistics)->addStatisticsTimeAndCount(m_type, m_interName, m_qubitNum, m_timeHandle());
}

void CStatisticsTimeAndCount::setExecType(const ExecCmdType::type type)
{
    m_type = type;
}

void CStatisticsTimeAndCount::setQubitNum(const int qubitNum)
{
    m_qubitNum = qubitNum;
}

CStatisticsCodeCount::CStatisticsCodeCount(const ExecCmdType::type type, const std::string& interName, const int qubitNum, const int code)
:m_type(type), m_interName(interName), m_qubitNum(qubitNum), m_code(code)
{

}

CStatisticsCodeCount::~CStatisticsCodeCount()
{
    SINGLETON(CStatistics)->addStatisticsCodeCount(m_type, m_interName, m_qubitNum, m_code);
}
