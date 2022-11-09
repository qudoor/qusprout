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

#include <sstream>
#include "Config.h"
#include "yaml-cpp/yaml.h"

CConfig::CConfig()
{
}

CConfig::~CConfig()
{
}

int CConfig::ParseConfig(const std::string& configFile)
{
    int nRet = 0;
    try
    {
        YAML::Node handle = YAML::LoadFile(configFile);
        if (handle["common"].IsDefined())
        {
            if (handle["common"]["verbose"].IsDefined())
                m_verbose = handle["common"]["verbose"].as<int>();
        }

        if (handle["log"].IsDefined())
        {
            if (handle["log"]["appname"].IsDefined())
                m_appname = handle["log"]["appname"].as<std::string>();

            if (handle["log"]["logpath"].IsDefined())
                m_logPath = handle["log"]["logpath"].as<std::string>();

            if (handle["log"]["loglevel"].IsDefined())
                m_logLevel = handle["log"]["loglevel"].as<int>();
        }
    }
    catch(const std::exception& e)
    {
        nRet = -1;
    }
    
    return nRet;
}

std::string CConfig::GetPrintInfo()
{
    std::ostringstream os("");
    os  << "prase config{"

        << "common[verbose:" << m_verbose
        << "];"

        << "log[appname:" << m_appname
        << ",logPath:" << m_logPath
        << ",loglevel:" << m_logLevel
        << "];"

        << "}.";

    return os.str();
}