#ifndef __H__STATISTICS__H__
#define __H__STATISTICS__H__

#include <chrono>
#include <thread>
#include <mutex>
#include "interface/qusproutdata_types.h"
#include "interface/statistics_types.h"

const std::string Tag_Iinterface_initQubits = "initQubits";
const std::string Tag_Iinterface_sendCircuitCmd = "sendCircuitCmd";
const std::string Tag_Iinterface_cancelCmd = "cancelCmd";
const std::string Tag_Iinterface_getProbAmp = "getProbAmp";
const std::string Tag_Iinterface_getProbOfOutcome = "getProbOfOutcome";
const std::string Tag_Iinterface_getProbOfAllOutcome = "getProbOfAllOutcome";
const std::string Tag_Iinterface_getAllState = "getAllState";
const std::string Tag_Iinterface_run = "run";
const std::string Tag_Iinterface_applyQFT = "applyQFT";
const std::string Tag_Iinterface_applyFullQFT = "applyFullQFT";
const std::string Tag_Iinterface_getExpecPauliProd = "getExpecPauliProd";
const std::string Tag_Iinterface_getExpecPauliSum = "getExpecPauliSum";

class CElapsedTime
{
public:
    CElapsedTime();
    ~CElapsedTime();

    //获取耗时
    int operator()();

private:
    std::chrono::time_point<std::chrono::steady_clock> m_start;
};

class CStatistics
{
public:
    CStatistics();
    ~CStatistics();

    //初始化
    int init();

    //接口调用耗时统计/调用次数统计/总任务数统计
    void addStatisticsTimeAndCount(const ExecCmdType::type type, const std::string& interName, const int qubitNum, const int elapsedTime);

    //接口调用返回码次数统计
    void addStatisticsCodeCount(const ExecCmdType::type type, const std::string& interName, const int qubitNum, const int code);

    //获取统计信息
    void getStatisticsInfo(StatisticsInfo& sis);

private:
    //获取耗时时间范围
    int getelapsedTimeRange(const int elapsedTime);

private:
    std::mutex m_mutex;

    //开始时间
    time_t m_beginTime{0};

    //总任务数统计
    std::map<std::string, SisAllTaskNum> m_allTaskNum;

    //接口调用次数
    std::map<std::string, SisInterfaceCount> m_interCount;

    //接口调用耗时
    std::map<std::string, SisInterfaceTime> m_interTime;

    //接口调用返回码次数
    std::map<std::string, SisCodeCount> m_codeCount;
};

//接口调用耗时统计/调用次数统计/总任务数统计
class CStatisticsTimeAndCount
{
public:
    CStatisticsTimeAndCount(const ExecCmdType::type type, const std::string& interName, const int qubitNum = 0);
    ~CStatisticsTimeAndCount();
    void setExecType(const ExecCmdType::type type);
    void setQubitNum(const int qubitNum);

private:
    ExecCmdType::type m_type;
    const std::string& m_interName;
    int m_qubitNum;
    CElapsedTime m_timeHandle;
};

//接口调用返回码次数统计
class CStatisticsCodeCount
{
public:
    CStatisticsCodeCount(const ExecCmdType::type type, const std::string& interName, const int qubitNum, const int code);
    ~CStatisticsCodeCount();

private:
    ExecCmdType::type m_type;
    const std::string& m_interName;
    const int m_qubitNum;
    const int m_code;
};

#endif
