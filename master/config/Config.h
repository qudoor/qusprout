#ifndef __H__CONFIG__H__
#define __H__CONFIG__H__

#include <string>
#include <map>
#include <set>

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

    //定时清理任务的间隔，单位：秒
    int m_cleanTaskInterval{30};

    //rpc的地址
    std::string m_listenAddr{""};

    //rpc监听的端口
    int m_listenPort{9090};

    //rpc发送超时
    int m_sendTimeout{0};

    //rpc接收超时
    int m_recvTimeout{0};

    //rpc系统指令监听的端口
    int m_listenSysPort{0};

    //客户端连接超时
    int m_clientConnTimeout{0};

    //客户端发送超时
    int m_clientSendTimeout{0};

    //客户端接收超时
    int m_clientRecvTimeout{0};

    //quroot开关
    int m_qurootEnable{0};

    //quroot的端口
    int m_qurootPort{0};

    //quroot的地址
    std::string m_qurootAddr{""};

    //quroot的心跳间隔
    int m_qurootHeartInterval{3};

    //设备支持的量子门
    std::map<std::string, std::set<std::string>> m_supportGate;

    //提供http的端口
    int m_listenHttpPort{8001};

    //http线程数
    int m_httpThreadNum{0};

    //任务清理超时时间，单位：秒
    std::map<int, int> m_cleanTaskTimeout;

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
};

#endif
