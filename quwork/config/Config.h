#ifndef __H__CONFIG__H__
#define __H__CONFIG__H__

#include <string>

class CConfig
{
public:
    CConfig();
    ~CConfig();

    int ParseConfig(const std::string& configFile);
    std::string GetPrintInfo();

public:
    //是否打印
    int m_verbose{0};

    //日志级别（0：INFO， 1：WARNING 2：ERROR 3：FATAL）
    int m_logLevel{0};

    //日志路径
    std::string m_logPath{"./"};

    //日志名称前缀
    std::string m_appname{"quwork"};
};

#endif
