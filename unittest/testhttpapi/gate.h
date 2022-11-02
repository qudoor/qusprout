#ifndef __H__GATE__H__
#define __H__GATE__H__

#include <string>
#include <vector>
#include <atomic>
#include "struct.h"

//任务状态
enum TaskState 
{
    //初始状态
    TASK_STATE_INITIAL = 0,

    //已初始化
    TASK_STATE_INITIALIZED = 1,

    //队列中
    TASK_STATE_QUEUED = 2,

    //执行任务中
    TASK_STATE_RUNNING = 3,

    //任务已完成
    TASK_STATE_DONE = 4,

    //任务出现异常
    TASK_STATE_ERROR = 5,

    //任务超时
    TASK_STATE_TIMEOUT = 6
};

class CGate
{
public:
    CGate();
    ~CGate();
    bool init(const int qubitnum, const bool density = false, const int exectype = 1, const int index = 0, const std::string& ip = "127.0.0.1", const int port = 9093);

public:
    bool createQCircuit();
    bool createPlusState();
    bool getProbAmp(const int index, double& amp);
    bool run(const int shots, RunGateCmdResp& runresp);
    bool getProb(const std::vector<int>& targets, std::vector<double>& outcomes);
    bool getState(std::vector<std::string>& states);
    bool getTask(int& state);
    bool getMeasure(const std::vector<int>& targets, GetMeasureResp& getresp);
    bool applyQftReq(const std::vector<int>& targets);
    bool getEPauli(const std::vector<PauliInfo>& paulis, double& expect);
    bool getEPauliSum(const std::vector<int>& opertypes, const std::vector<double>& terms, double& expect);
    bool releaseQCircuit();
    bool getMetrics(std::string& metrics);

public:
    bool hGate(const std::vector<int>& targets);
    bool mczGate(const std::vector<int>& controls, const std::vector<int>& targets);
    bool xGate(const std::vector<int>& targets);
    bool cnotGate(const std::vector<int>& controls, const std::vector<int>& targets);
    bool measureGate(const std::vector<int>& targets);

    bool endAdd();
private:
    std::string getSeq();

    template <typename T, typename K>
    bool sendclient(T& req, K& resp);

private:
    int m_qubitnum{0};
    bool m_density{false};
    int m_exectype{0};
    std::string m_ip{""};
    int m_port{0};
    std::string m_taskid{""};
    std::atomic_long m_index{0};
    std::atomic_long m_seq{0};
    bool m_isrelease{false};
    int m_taskIndex{0};
};

#endif
