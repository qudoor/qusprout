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

#ifndef __H__METRICS__H__
#define __H__METRICS__H__

#include <string>
#include <memory>
#include <chrono>
#include <map>
#include <prometheus/counter.h>
#include <prometheus/gauge.h>
#include <prometheus/histogram.h>
#include <prometheus/registry.h>
#include <prometheus/text_serializer.h>
#include "interface/ecode_types.h"
#include "common/SystemHandle.h"

class CTaskStateMetrics
{
public:
    std::map<std::string, int> m_state;
};

class CMetricsHandle 
{
public:
	CMetricsHandle(std::shared_ptr<prometheus::Registry> registry);
	~CMetricsHandle();

public:
    //每个任务创建到结束的耗时
    prometheus::Family<prometheus::Histogram>& m_taskEscapeTime;

    //每个机器的累计任务数量
    prometheus::Family<prometheus::Counter>& m_taskCount;

    //每个接口的耗时
    prometheus::Family<prometheus::Histogram>& m_interfaceEscapeTime;

    //每个错误码的累计数量
    prometheus::Family<prometheus::Counter>& m_codeCount;

    //任务最终的状态的数量
    prometheus::Family<prometheus::Counter>& m_taskState;

    //当前任务的每个状态的数量
    prometheus::Family<prometheus::Gauge>& m_currTaskState;

    //资源的总内存
    prometheus::Family<prometheus::Gauge>& m_currRsAllMemory;

    //资源的可用内存
    prometheus::Family<prometheus::Gauge>& m_currRsFreeMemory;

    //Gpu资源的显存类型
    prometheus::Family<prometheus::Gauge>& m_currGpuType;

    //Gpu资源的总显存
    prometheus::Family<prometheus::Gauge>& m_currGpuAllMemory;

    //Gpu资源的可用显存
    prometheus::Family<prometheus::Gauge>& m_currGpuFreeMemory;

    //资源的更新时间
    prometheus::Family<prometheus::Gauge>& m_currRsUpdatetime;

    //进程启动时间
    prometheus::Family<prometheus::Gauge>& m_currStartTime;
};

class CMetrics 
{
public:
	CMetrics();
	~CMetrics();

    int init();

    void addTaskEscapeTime(const std::string& qubits, const double ms);
    void addTaskCount(const std::string& qubits);
    void addInterfaceEscapeTime(const std::string& interfacename, const double ms);
    void addCodeCount(const std::string& interfacename, const std::string& code);
    void addTaskState(const std::string& qubits, const std::string& state);
    void addResource(const MachineSysInfo& sys, const MemUseInfo& mem, const GpuUseInfo& gpu);
    void updateStarttime(const int currtime);

    void addCurrTaskState(const CTaskStateMetrics& key);
    std::string getMetricsStr();

private:
    std::shared_ptr<prometheus::Registry> m_registry{nullptr};
    std::shared_ptr<CMetricsHandle> m_metrics{nullptr};
};

class CMetricsGuard
{
public:
    CMetricsGuard(const std::string& interfacename, const int& code);
    ~CMetricsGuard();

private:
    const std::string& m_interfacename;
    const int& m_code;
    long long int m_time;
};

#endif