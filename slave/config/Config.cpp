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
        if (handle["common"]["processName"].IsDefined())
            m_processName = handle["common"]["processName"].as<std::string>();
    }

    if (handle["rpc"].IsDefined())
    {
        if (handle["rpc"]["listenPort"].IsDefined())
            m_listenPort = handle["rpc"]["listenPort"].as<int>();

        if (handle["rpc"]["sendTimeout"].IsDefined())
            m_sendTimeout = handle["rpc"]["sendTimeout"].as<int>();

        if (handle["rpc"]["recvTimeout"].IsDefined())
            m_recvTimeout = handle["rpc"]["recvTimeout"].as<int>();
    }

    if (handle["task"].IsDefined())
    {
        if (handle["task"]["cleanTaskInterval"].IsDefined())
            m_cleanTaskInterval = handle["task"]["cleanTaskInterval"].as<int>();

        if (handle["task"]["taskTimeOutDuration"].IsDefined())
            m_taskTimeOutDuration = handle["task"]["taskTimeOutDuration"].as<int>();
    }

    if (handle["register"].IsDefined())
    {
        if (handle["register"]["masterHeartInterval"].IsDefined())
            m_masterHeartInterval = handle["register"]["masterHeartInterval"].as<int>();
    }

    if (handle["master"].IsDefined())
    {
        if (handle["master"]["masterPort"].IsDefined())
            m_masterPort = handle["master"]["masterPort"].as<int>();

        if (handle["master"]["masterAddr"].IsDefined())
            m_masterAddr = handle["master"]["masterAddr"].as<std::string>();
    }

    if (handle["work"].IsDefined())
    {
        if (handle["work"]["workBinPath"].IsDefined())
            m_workBinPath = handle["work"]["workBinPath"].as<std::string>();

        if (handle["work"]["workBinName"].IsDefined())
            m_workBinName = handle["work"]["workBinName"].as<std::string>();

        if (handle["work"]["workConfigPath"].IsDefined())
            m_workConfigPath = handle["work"]["workConfigPath"].as<std::string>();
        
        if (handle["work"]["workConfigName"].IsDefined())
            m_workConfigName = handle["work"]["workConfigName"].as<std::string>();

        if (handle["work"]["waitRpcTimeout"].IsDefined())
            m_waitRpcTimeout = handle["work"]["waitRpcTimeout"].as<int>();

        if (handle["work"]["waitMpiRpcTimeout"].IsDefined())
            m_waitMpiRpcTimeout = handle["work"]["waitMpiRpcTimeout"].as<int>();
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

        << "common[processName:" << m_processName
        << "];"

        << "rpc[listenPort:" << m_listenPort
        << ",sendTimeout:" << m_sendTimeout
        << ",recvTimeout:" << m_recvTimeout
        << "];"
 
        << "task[cleanTaskInterval:" << m_cleanTaskInterval
        << ",taskTimeOutDuration:" << m_taskTimeOutDuration
        << "];"

        << "register[masterHeartInterval:" << m_masterHeartInterval
        << "];"

        << "master[masterPort:" << m_masterPort
        << ",masterAddr:" << m_masterAddr
        << "];"

        << "work[workBinPath:" << m_workBinPath
        << ",workBinName:" << m_workBinName
        << ",workConfigPath:" << m_workConfigPath
        << ",workConfigName:" << m_workConfigName
        << ",waitRpcTimeout:" << m_waitRpcTimeout
        << ",waitMpiRpcTimeout:" << m_waitMpiRpcTimeout
        << "];"

        << "client[clientConnTimeout:" << m_clientConnTimeout
        << ",clientSendTimeout:" << m_clientSendTimeout
        << ",clientRecvTimeout:" << m_clientRecvTimeout
        << "];"

        << "}.";

    return os.str();
}