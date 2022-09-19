#ifndef QULOG_H
#define QULOG_H

#include <string>
#include <glog/logging.h>

class CQuLog
{
public:
    CQuLog();
    virtual ~CQuLog();

    bool Init(std::string szAppName, std::string szLogPath, int nMinLogLevel);
};

#endif