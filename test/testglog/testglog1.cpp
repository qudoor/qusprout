#ifndef GLOG_NO_ABBREVIATED_SEVERITIES
#define GLOG_NO_ABBREVIATED_SEVERITIES // 如果不加这个宏定义代码就会报错
#endif
#include <glog/logging.h>

int main(int argc, char* argv[])
{
	FLAGS_alsologtostderr = false;//是否将日志输出到文件和stderr
	google::InitGoogleLogging("test");//使用glog之前必须先初始化库，仅需执行一次，括号内为程序名
	LOG(INFO) << "Hello,GLOG!";
	google::ShutdownGoogleLogging();//当要结束glog时必须关闭库，否则会内存溢出

	return 0;
}