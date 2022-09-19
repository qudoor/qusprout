#include <sstream>
#include "Config.h"
#include "yaml-cpp/yaml.h"

CConfig::CConfig()
{

}

CConfig::~CConfig()
{

}

int CConfig::praseConfig(const std::string& configFile)
{
    YAML::Node handle = YAML::LoadFile(configFile);

    if (handle["log"].IsDefined())
    {
        if (handle["log"]["logLevel"].IsDefined())
            m_logLevel = handle["log"]["logLevel"].as<int>();

        if (handle["log"]["logPath"].IsDefined())
            m_logPath = handle["log"]["logPath"].as<std::string>();

        if (handle["log"]["logNamePrefix"].IsDefined())
            m_logNamePrefix = handle["log"]["logNamePrefix"].as<std::string>();
    }

    if (handle["common"].IsDefined())
    {
        if (handle["common"]["verbose"].IsDefined())
            m_verbose = handle["common"]["verbose"].as<int>();
    }

    
    if (handle["rpc"].IsDefined())
    {
        if (handle["rpc"]["listenPort"].IsDefined())
            m_listenPort = handle["rpc"]["listenPort"].as<int>();

        if (handle["rpc"]["sendTimeout"].IsDefined())
            m_sendTimeout = handle["rpc"]["sendTimeout"].as<int>();

        if (handle["rpc"]["recvTimeout"].IsDefined())
            m_recvTimeout = handle["rpc"]["recvTimeout"].as<int>();

        if (handle["rpc"]["listenSysPort"].IsDefined())
            m_listenSysPort = handle["rpc"]["listenSysPort"].as<int>();
    }

    if (handle["task"].IsDefined())
    {
        if (handle["task"]["cleanTaskInterval"].IsDefined())
            m_cleanTaskInterval = handle["task"]["cleanTaskInterval"].as<int>();

        if (handle["task"]["taskTimeOutDuration"].IsDefined())
            m_taskTimeOutDuration = handle["task"]["taskTimeOutDuration"].as<int>();
    }

    if (handle["resource"].IsDefined())
    {
        if (handle["resource"]["cleanResourceInterval"].IsDefined())
            m_cleanResourceInterval = handle["resource"]["cleanResourceInterval"].as<int>();

        if (handle["resource"]["resourceTimeOutDuration"].IsDefined())
            m_resourceTimeOutDuration = handle["resource"]["resourceTimeOutDuration"].as<int>();

        if (handle["resource"]["resourceHeartInterval"].IsDefined())
            m_resourceHeartInterval = handle["resource"]["resourceHeartInterval"].as<int>();
    }

    if (handle["client"].IsDefined())
    {
        if (handle["client"]["clientConnTimeout"].IsDefined())
            m_clientConnTimeout = handle["client"]["clientConnTimeout"].as<int>();

        if (handle["client"]["clientSendTimeout"].IsDefined())
            m_clientSendTimeout = handle["client"]["clientSendTimeout"].as<int>();

        if (handle["client"]["clientRecvTimeout"].IsDefined())
            m_clientRecvTimeout = handle["client"]["clientRecvTimeout"].as<int>();
    }

    return 0;
}

std::string CConfig::getPrintStr()
{
    std::ostringstream os("");
    os << "prase config{"

        << "log[logLevel:" << m_logLevel
        << ",logPath:" << m_logPath
        << ",logNamePrefix:" << m_logNamePrefix
        << "];"

        << "common[verbose:" << m_verbose
        << "];"

        << "rpc[listenPort:" << m_listenPort
        << ",sendTimeout:" << m_sendTimeout
        << ",recvTimeout:" << m_recvTimeout
        << ",listenSysPort:" << m_listenSysPort
        << "];"

        << "task[cleanTaskInterval:" << m_cleanTaskInterval
        << ",taskTimeOutDuration:" << m_taskTimeOutDuration
        << "];"

        << "resource[cleanResourceInterval:" << m_cleanResourceInterval
        << ",resourceTimeOutDuration:" << m_resourceTimeOutDuration
        << ",resourceHeartInterval:" << m_resourceHeartInterval
        << "];"

        << "client[clientConnTimeout:" << m_clientConnTimeout
        << ",clientSendTimeout:" << m_clientSendTimeout
        << ",clientRecvTimeout:" << m_clientRecvTimeout
        << "];"

        << "}.";

    return os.str();
}