#ifndef UTILITY_H
#define UTILITY_H

using namespace std;

class Config {
public:
    int port;
    // int qubits;
    bool verbose;

    //任务组数量
    int taskArraryNum{5};

    //获取资源定时器定时时长，默认9秒
    int getResourceDuration{9};

    //获取资源超时时间，默认10秒
    int getResourceTimeoutDuration{10};

    //清理任务定时器定时时长，默认60秒
    int cleanTaskDuration{60};

    //任务超时时间，默认3600秒
    int taskTimeoutDuration{3600};

    //接口服务启动线程数，默认10
    int thriftThreadNum{10};
    
public:
    Config() {
        port = 9090;
        // qubits = 20;
        verbose = false;
    }
    ~Config() = default;
};

#endif // UTILITY_H