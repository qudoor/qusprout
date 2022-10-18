#include "metrics.h"
#include "common/qulog.h"
#include "comm/Base.h"
#include "common/Singleton.h"

CMetricsHandle::CMetricsHandle(std::shared_ptr<prometheus::Registry> registry)
:m_taskEscapeTime(
    prometheus::BuildHistogram()
    .Name("qusprout_task_duration_ms")
    .Help("qusprout_task_duration_ms histogram")
    .Register(*registry)
),
m_taskCount(
    prometheus::BuildCounter()
    .Name("qusprout_task_count")
    .Help("qusprout_task_count counter")
    .Register(*registry)
),
m_interfaceEscapeTime(
    prometheus::BuildHistogram()
    .Name("qusprout_interface_duration_ms")
    .Help("qusprout_interface_duration_ms histogram")
    .Register(*registry)
),
m_codeCount(
    prometheus::BuildCounter()
    .Name("qusprout_code")
    .Help("qusprout_code counter")
    .Register(*registry)
),
m_taskState(
    prometheus::BuildCounter()
    .Name("qusprout_task_state")
    .Help("qusprout_task_state counter")
    .Register(*registry)
),
m_currTaskState(
    prometheus::BuildGauge()
    .Name("qusprout_task_curr_state")
    .Help("qusprout_task_curr_state gauge")
    .Register(*registry)
),
m_currRsAllMemory(
    prometheus::BuildGauge()
    .Name("qusprout_resource_curr_cpu_all_memory")
    .Help("qusprout_resource_curr_cpu_all_memory gauge")
    .Register(*registry)
),
m_currRsFreeMemory(
    prometheus::BuildGauge()
    .Name("qusprout_resource_curr_cpu_free_memory")
    .Help("qusprout_resource_curr_cpu_free_memory gauge")
    .Register(*registry)
),
m_currRsGpuType(
    prometheus::BuildGauge()
    .Name("qusprout_resource_curr_gpu_type")
    .Help("qusprout_resource_curr_gpu_type gauge")
    .Register(*registry)
),
m_currRsGpuAllMemory(
    prometheus::BuildGauge()
    .Name("qusprout_resource_curr_gpu_all_memory")
    .Help("qusprout_resource_curr_gpu_all_memory gauge")
    .Register(*registry)
),
m_currRsUpdatetime(
    prometheus::BuildGauge()
    .Name("qusprout_resource_curr_updatetime")
    .Help("qusprout_resource_curr_updatetime gauge")
    .Register(*registry)
),
m_currRsState(
    prometheus::BuildGauge()
    .Name("qusprout_resource_curr_state")
    .Help("qusprout_resource_curr_state gauge")
    .Register(*registry)
),
m_currStartTime(
    prometheus::BuildGauge()
    .Name("qusprout_curr_starttime")
    .Help("qusprout_curr_starttime gauge")
    .Register(*registry)
)
{
}

CMetricsHandle::~CMetricsHandle()
{

}

CMetrics::CMetrics()
{

}

CMetrics::~CMetrics()
{

}

int CMetrics::init()
{
    m_registry = std::make_shared<prometheus::Registry>();    
    m_metrics = std::make_shared<CMetricsHandle>(m_registry);
    return 0;
}

void CMetrics::addTaskEscapeTime(const std::string& resourceid, const std::string& qubits, const double ms)
{
    if (!resourceid.empty() && !qubits.empty())
    {
        auto& histogram = m_metrics->m_taskEscapeTime.Add(
            {
                {"resourceid", resourceid},
                {"qubits", qubits}
            },
            prometheus::Histogram::BucketBoundaries{ 5.00, 10.00, 50.00, 100.00, 250.00, 500.00, 1000.00, 2000.00, 3000.00, 5000.00, 10000.00, 20000.00, 30000.00 }
        );

        histogram.Observe(ms);
    }
}

void CMetrics::addTaskCount(const std::string& resourceid, const std::string& qubits)
{
    auto& count = m_metrics->m_taskCount.Add(
        {
            {"resourceid", resourceid},
            {"qubits", qubits}
        }
    );

    count.Increment();
}

void CMetrics::addInterfaceEscapeTime(const std::string& interfacename, const double ms)
{
    auto& histogram = m_metrics->m_interfaceEscapeTime.Add(
        {
            {"interfacename", interfacename}
        },
        prometheus::Histogram::BucketBoundaries{ 5.00, 10.00, 50.00, 100.00, 250.00, 500.00, 1000.00, 2000.00, 3000.00, 5000.00, 10000.00, 20000.00, 30000.00 }
    );

	histogram.Observe(ms);
}

void CMetrics::addCodeCount(const std::string& interfacename, const std::string& code)
{
    auto& count = m_metrics->m_codeCount.Add(
        {
            {"interfacename", interfacename},
            {"code", code}
        }
    );

    count.Increment();
}

void CMetrics::addTaskState(const std::string& resourceid, const std::string& qubits, const std::string& state)
{
    auto& count = m_metrics->m_taskState.Add(
        {
            {"resourceid", resourceid},
            {"qubits", qubits},
            {"state", state}
        }
    );

    count.Increment();
}

void CMetrics::addCurrTaskState(const CTaskStateMetrics& key)
{
    auto iter = key.m_state.begin();
    for (; iter != key.m_state.end(); ++iter)
    {
        auto& gauge = m_metrics->m_currTaskState.Add(
            {
                {"state", iter->first}
            }
        );

        gauge.Set(iter->second);
    }
}

void CMetrics::addResource(const std::string& resourceid, const DeviceResourceDetail& resource)
{
    auto& machine = resource.machine;
    auto& resourceinfo = resource.resource;

    double cpuallmemory = resourceinfo.cpu_total_memory;
    double cpufreememory = resourceinfo.cpu_free_memory;
    double gputype = resourceinfo.gpu_type;
    double gpuallmemory = resourceinfo.gpu_total_memory;

    //资源的总内存
    auto& allcpumemorygauge = m_metrics->m_currRsAllMemory.Add(
        {
            {"resourceid", resourceid},
            {"addr", machine.addr},
            {"sysname", machine.sys_name},
            {"sysrelease", machine.sys_release},
            {"sysversion", machine.sys_version},
            {"sysmachine", machine.sys_machine}
        }
    );
    allcpumemorygauge.Set(cpuallmemory);
        
    //资源的可用内存
    auto& freecpumemorygauge = m_metrics->m_currRsFreeMemory.Add(
        {
            {"resourceid", resourceid},
            {"addr", machine.addr},
            {"sysname", machine.sys_name},
            {"sysrelease", machine.sys_release},
            {"sysversion", machine.sys_version},
            {"sysmachine", machine.sys_machine}
        }
    );
    freecpumemorygauge.Set(cpufreememory);

    //资源的显存类型
    auto& gputypegauge = m_metrics->m_currRsGpuType.Add(
        {
            {"resourceid", resourceid},
            {"addr", machine.addr},
            {"sysname", machine.sys_name},
            {"sysrelease", machine.sys_release},
            {"sysversion", machine.sys_version},
            {"sysmachine", machine.sys_machine}
        }
    );
    gputypegauge.Set(gputype);

    //资源的总显存
    auto& allgpumemorygauge = m_metrics->m_currRsGpuAllMemory.Add(
        {
            {"resourceid", resourceid},
            {"addr", machine.addr},
            {"sysname", machine.sys_name},
            {"sysrelease", machine.sys_release},
            {"sysversion", machine.sys_version},
            {"sysmachine", machine.sys_machine}
        }
    );
    allgpumemorygauge.Set(gpuallmemory);

    //资源的启动时间
    auto& starttimegauge = m_metrics->m_currRsUpdatetime.Add(
        {
            {"resourceid", resourceid},
            {"addr", machine.addr},
            {"sysname", machine.sys_name},
            {"sysrelease", machine.sys_release},
            {"sysversion", machine.sys_version},
            {"sysmachine", machine.sys_machine}
        }
    );
    starttimegauge.SetToCurrentTime();

    //资源的状态
    auto& stategauge = m_metrics->m_currRsState.Add(
        {
            {"resourceid", resourceid}
        }
    );
    stategauge.SetToCurrentTime();
}

void CMetrics::clearResource(const std::string& resourceid)
{
    auto& stategauge = m_metrics->m_currRsState.Add(
        {
            {"resourceid", resourceid}
        }
    );
    stategauge.Set(0);
}

void CMetrics::updateStarttime(const int currtime)
{
    auto& starttimeauge = m_metrics->m_currStartTime.Add(
        {
        }
    );
    starttimeauge.Set(currtime);
}

std::string CMetrics::getMetricsStr()
{
    auto taskescapemetrics = m_metrics->m_taskEscapeTime.Collect();
    auto countmetrics = m_metrics->m_taskCount.Collect();
    auto interfacemetrics = m_metrics->m_interfaceEscapeTime.Collect();
    auto codemetrics = m_metrics->m_codeCount.Collect();
    auto statemetrics = m_metrics->m_taskState.Collect();
    auto currstatemetrics = m_metrics->m_currTaskState.Collect();
    auto rsallmrmorymetrics = m_metrics->m_currRsAllMemory.Collect();
    auto rsfreememorymetrics = m_metrics->m_currRsFreeMemory.Collect();
    auto rsgputypemetrics = m_metrics->m_currRsGpuType.Collect();
    auto rsgpuallmemorymetrics = m_metrics->m_currRsGpuAllMemory.Collect();
    auto rsstarttimemetrics = m_metrics->m_currRsUpdatetime.Collect();
    auto rsstatemetrics = m_metrics->m_currRsState.Collect();
    auto starttimemetrics = m_metrics->m_currStartTime.Collect();

    taskescapemetrics.insert(taskescapemetrics.end(), countmetrics.begin(), countmetrics.end());
    taskescapemetrics.insert(taskescapemetrics.end(), interfacemetrics.begin(), interfacemetrics.end());
    taskescapemetrics.insert(taskescapemetrics.end(), codemetrics.begin(), codemetrics.end());
    taskescapemetrics.insert(taskescapemetrics.end(), statemetrics.begin(), statemetrics.end());
    taskescapemetrics.insert(taskescapemetrics.end(), currstatemetrics.begin(), currstatemetrics.end());
    taskescapemetrics.insert(taskescapemetrics.end(), rsallmrmorymetrics.begin(), rsallmrmorymetrics.end());
    taskescapemetrics.insert(taskescapemetrics.end(), rsfreememorymetrics.begin(), rsfreememorymetrics.end());
    taskescapemetrics.insert(taskescapemetrics.end(), rsgputypemetrics.begin(), rsgputypemetrics.end());
    taskescapemetrics.insert(taskescapemetrics.end(), rsgpuallmemorymetrics.begin(), rsgpuallmemorymetrics.end());
    taskescapemetrics.insert(taskescapemetrics.end(), rsstarttimemetrics.begin(), rsstarttimemetrics.end());
    taskescapemetrics.insert(taskescapemetrics.end(), rsstatemetrics.begin(), rsstatemetrics.end());
    taskescapemetrics.insert(taskescapemetrics.end(), starttimemetrics.begin(), starttimemetrics.end());

    const auto serializer = prometheus::TextSerializer{};
    auto body = serializer.Serialize(taskescapemetrics);
    return body;
}

CMetricsGuard::CMetricsGuard(const std::string& interfacename, const int& code)
:m_interfacename(interfacename), m_code(code)
{
    m_time = getCurrMs();
}

CMetricsGuard::~CMetricsGuard()
{
    int ms = getCurrMs() - m_time;
    std::ostringstream os("");
    os << m_code;
    if (ms >= 25000)
    {
        LOG(INFO) << "CMetricsGuard(interfacename:" << m_interfacename << ",code:" << m_code << ",ms:" << ms << ").";
    }
    SINGLETON(CMetrics)->addInterfaceEscapeTime(m_interfacename, ms);
    SINGLETON(CMetrics)->addCodeCount(m_interfacename, os.str());
}
