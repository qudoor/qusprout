#ifndef __H__CONFIG__H__
#define __H__CONFIG__H__

#include <string>

class CConfig
{
public:
    CConfig();
    ~CConfig();

    int praseConfig(const std::string& configFile);
    std::string getPrintStr();

public:
    //日志级别（0：INFO， 1：WARNING 2：ERROR 3：FATAL）
    int m_logLevel{0};

    //日志路径
    std::string m_logPath{"./config"};

    //日志名称前缀
    std::string m_logNamePrefix{"master"};

    //进程名称
    std::string m_processName;

    //rpc监听的端口
    int m_listenPort{9091};

    //rpc发送超时
    int m_sendTimeout{0};

    //rpc接收超时
    int m_recvTimeout{0};

    //定时清理任务的间隔，单位：秒
    int m_cleanTaskInterval{30};

    //任务超时时长，单位：秒
    int m_taskTimeOutDuration{3600};

    //和master心跳间隔
    int m_masterHeartInterval{10};

    //master的rpc端口
    int m_masterPort{0};

    //master的rpc的地址
    std::string m_masterAddr{""};

    //work的执行文件全路径
    std::string m_workBinPath{""};

    //work的执行文件名称
    std::string m_workBinName{""};

    //work的执行配置文件路径
    std::string m_workConfigPath{""};

    //work的执行配置文件名称
    std::string m_workConfigName{""};

    //等待work的rpc启动的超时时间，单位：秒
    int m_waitRpcTimeout{0};

    //等待work的mpi方式rpc启动的超时时间，单位：秒
    int m_waitMpiRpcTimeout{0};

    //客户端连接超时
    int m_clientConnTimeout{0};

    //客户端发送超时
    int m_clientSendTimeout{0};

    //客户端接收超时
    int m_clientRecvTimeout{0};
};

#endif
