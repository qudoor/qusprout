#include <iostream>
#include <fstream>
#include <sstream>
#include <unistd.h>
#include <string>
#include <iomanip>
#include <sys/types.h>
#include <unistd.h>
#include <thread>
#include <glog/logging.h>

//将信息输出到单独的文件和 LOG(ERROR)
void SignalHandle(const char* data, size_t size)
{
    std::ofstream fs("glog_dump.log",std::ios::app);
    std::string str = std::string(data, size);
    fs << str;
    fs.close();
    LOG(ERROR) << str;
}

void CustomPrefix(std::ostream &s, const google::LogMessageInfo &l, void*) {
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
    << ' '
    << std::this_thread::get_id()
    << "["
    << l.severity[0]
    << "]["
    << l.filename << ':' << l.line_number << "]";
}

int main(int argc, char** argv)
{
    //初始化
    google::InitGoogleLogging(argv[0], &CustomPrefix); 
    
    //设置日志消息是否转到标准输出而不是日志文件
    FLAGS_logtostderr = false; 

    //设置日志消息除了日志文件之外是否去标准输出
    FLAGS_alsologtostderr = false; 

    //最大日志大小为 10M                      
    FLAGS_max_log_size = 10;   

    //当磁盘被写满时，停止日志输出                 
    FLAGS_stop_logging_if_full_disk = true;     

    //日志目录
    FLAGS_log_dir = "./";

    //用来设置最大的显示等级，需要结合VLOG宏来一起使用
    FLAGS_minloglevel = google::INFO;

    //文件名是否使用时间戳
    FLAGS_timestamp_in_logfile_name = false;

    //设置日志文件名后缀
    google::SetLogFilenameExtension(".log");

    //日志文件保留30天
    google::EnableLogCleaner(30);

    std::ostringstream os("");
    os << argv[0];
    
    //设置级别的日志存储路径和文件名前缀
    //os << argv[0] << "_warn";
    google::SetLogDestination(google::GLOG_WARNING, os.str().c_str());  
    //os.str("");
    //os << argv[0] << "_info";
    google::SetLogDestination(google::GLOG_INFO, os.str().c_str()); 
    //os.str("");
    //os << argv[0] << "_err";
    google::SetLogDestination(google::GLOG_ERROR, os.str().c_str());
    //os.str("");
    //os << argv[0] << "_fat";
    google::SetLogDestination(google::GLOG_FATAL, os.str().c_str());        

    //输出日志
    LOG(INFO) << "info test";  
    LOG(WARNING) << "warning test getpid:" << getpid() << "," << std::this_thread::get_id();  
    LOG(ERROR) << "error test"; 
    //输出一个Fatal日志，这是最严重的日志并且输出之后会中止程序
    //LOG(FATAL) << "fatal test";  

    std::cout <<"-------end-----" << std::endl;
    google::InstallFailureSignalHandler();
    //默认捕捉 SIGSEGV 信号信息输出会输出到 stderr，能够通过以下的方法自己定义输出方式：
    google::InstallFailureWriter(&SignalHandle);
    google::ShutdownGoogleLogging();
    return 0;
}
