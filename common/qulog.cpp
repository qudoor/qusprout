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

#include <iostream>
#include <fstream>
#include <sstream>
#include <unistd.h>
#include <string>
#include <iomanip>
#include <sys/types.h>
#include "qulog.h"

void SignalHandle(const char* data, size_t size)
{
    std::ofstream fs("glog_dump.log",std::ios::app);
    std::string str = std::string(data, size);
    fs << str;
    fs.close();
}

void CustomPrefix(std::ostream &s, const google::LogMessageInfo &l, void*) 
{
    s << std::setw(4) << 1900 + l.time.year()
    << std::setw(2) << 1 + l.time.month()
    << std::setw(2) << l.time.day()
    << ' '
    << std::setw(2) << l.time.hour() << ':'
    << std::setw(2) << l.time.min()  << ':'
    << std::setw(2) << l.time.sec() << "."
    << std::setw(6) << l.time.usec()
    << ' '
    << l.thread_id
    << "["
    << l.severity[0]
    << "]["
    << l.filename << ':' << l.line_number << "]";
}

CQuLog::CQuLog()
{
}

CQuLog::~CQuLog()
{
    google::ShutdownGoogleLogging();
}

bool CQuLog::Init(std::string szAppName, std::string szLogPath, int nMinLogLevel)
{
    //初始化
    google::InitGoogleLogging(szAppName.c_str(), &CustomPrefix); 

    //设置日志消息是否转到标准输出而不是日志文件
    FLAGS_logtostderr = false; 

    //设置日志消息除了日志文件之外是否去标准输出
    FLAGS_alsologtostderr = false; 

    //最大日志大小为 10M                   
    FLAGS_max_log_size = 10;   

    //当磁盘被写满时，停止日志输出              
    FLAGS_stop_logging_if_full_disk = true;     
    
    //日志目录
    //FLAGS_log_dir = szLogPath;

    //用来设置最大的显示等级，需要结合VLOG宏来一起使用  
    FLAGS_minloglevel = nMinLogLevel;

    //文件名是否使用时间戳 
    FLAGS_timestamp_in_logfile_name = false;

    //设置可以缓冲日志的最大秒数，0指实时输出
    FLAGS_logbufsecs = 0;

    //设置日志文件名后缀
    google::SetLogFilenameExtension(".log");

    //日志文件保留30天
    google::EnableLogCleaner(30);
    
    //设置级别的日志存储全路径
    std::string szFullPath = szLogPath + "/" + szAppName;
    google::SetLogDestination(google::GLOG_WARNING, szFullPath.c_str());  
    google::SetLogDestination(google::GLOG_INFO, szFullPath.c_str()); 
    google::SetLogDestination(google::GLOG_ERROR, szFullPath.c_str());
    google::SetLogDestination(google::GLOG_FATAL, szFullPath.c_str());        

    //默认捕捉 SIGSEGV 信号信息输出会输出到 stderr，能够通过以下的方法自己定义输出方式：
    google::InstallFailureSignalHandler();
    google::InstallFailureWriter(&SignalHandle);

    return true;
}