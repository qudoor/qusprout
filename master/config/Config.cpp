#include <sstream>
#include "Config.h"
#include "yaml-cpp/yaml.h"

int split(std::vector<std::string>& datalist, const std::string& str, const std::string& delimitstr)
{
	unsigned long begin = 0, end = 0, len = 0;
	do
	{
		end	= str.find(delimitstr, begin);
		if (end == std::string::npos)
		    len = str.length() - begin;
		else
			len = end - begin;
		datalist.push_back(str.substr(begin, len));

		if (end == std::string::npos)
			break;
		begin = end + delimitstr.length();
	} while (1);

	return datalist.size();
}

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

    if (handle["rpc"].IsDefined())
    {
        if (handle["rpc"]["listenAddr"].IsDefined())
            m_listenAddr = handle["rpc"]["listenAddr"].as<int>();

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

        if (handle["task"]["cleanTaskTimeout"].IsDefined())
        {
            auto temp = handle["task"]["cleanTaskTimeout"];
            for (size_t i = 0; i < temp.size(); i++) 
            {
                int state = 0;
                int timeout = 0;
                if (temp[i]["taskState"].IsDefined())
                {
                    state = temp[i]["taskState"].as<int>();
                }
                if (temp[i]["timeout"].IsDefined())
                {
                    timeout = temp[i]["timeout"].as<int>();
                }
                m_cleanTaskTimeout[state] = timeout;
            }
        }
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

    if (handle["quroot"].IsDefined())
    {
        if (handle["quroot"]["enable"].IsDefined())
            m_qurootEnable = handle["quroot"]["enable"].as<int>();

        if (handle["quroot"]["port"].IsDefined())
            m_qurootPort = handle["quroot"]["port"].as<int>();

        if (handle["quroot"]["addr"].IsDefined())
            m_qurootAddr = handle["quroot"]["addr"].as<std::string>();

        if (handle["quroot"]["qurootHeartInterval"].IsDefined())
            m_qurootHeartInterval = handle["quroot"]["qurootHeartInterval"].as<int>();
    }
    
    if (handle["cap"].IsDefined())
    {
        auto temp = handle["cap"];
        for (size_t i = 0; i < temp.size(); i++) 
        {
            std::string devicetype = "";
            std::string supgate = "";
            if (temp[i]["deviceType"].IsDefined())
            {
                devicetype = temp[i]["deviceType"].as<std::string>();
            }
            if (temp[i]["supGate"].IsDefined())
            {
                supgate = temp[i]["supGate"].as<std::string>();
            }
            std::vector<std::string> gates;
            split(gates, supgate, ",");
            for (auto gate : gates)
            {
                m_supportGate[devicetype].insert(gate);
            }
        }
    }

    if (handle["http"].IsDefined())
    {
        if (handle["http"]["listenHttpPort"].IsDefined())
            m_listenHttpPort = handle["http"]["listenHttpPort"].as<int>();

        if (handle["http"]["httpThreadNum"].IsDefined())
            m_httpThreadNum = handle["http"]["httpThreadNum"].as<int>();
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

        << "rpc[listenPort:" << m_listenPort
        << ",listenAddr:" << m_listenAddr
        << ",sendTimeout:" << m_sendTimeout
        << ",recvTimeout:" << m_recvTimeout
        << ",listenSysPort:" << m_listenSysPort
        << "];"

        << "task[cleanTaskInterval:" << m_cleanTaskInterval;
    for (auto iter = m_cleanTaskTimeout.begin(); iter != m_cleanTaskTimeout.end(); ++iter)
    {
        os << ",taskState:" << iter->first << ",timeout:" << iter->second;
    }
    os << "];"

        << "client[clientConnTimeout:" << m_clientConnTimeout
        << ",clientSendTimeout:" << m_clientSendTimeout
        << ",clientRecvTimeout:" << m_clientRecvTimeout
        << "];"

        << "quroot[port:" << m_qurootPort
        << ",enable:" << m_qurootEnable
        << ",addr:" << m_qurootAddr
        << "];"

        << "cap[";
    for (auto iter = m_supportGate.begin(); iter != m_supportGate.end(); ++iter)
    {
        os << ";deviceType:" << iter->first
            << "gates:";
        for (auto gate : iter->second)
        {
            os << gate << "|";
        }
    }
    os << "];"

        << "http[listenHttpPort:" << m_listenHttpPort
        << ",httpThreadNum:" << m_httpThreadNum
        << "];"

        << "}.";

    return os.str();
}