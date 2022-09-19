#include <getopt.h>
#include <thread>
#include <functional>
#include <signal.h>
#include "common/qulog.h"
#include "config/Config.h"
#include "common/Singleton.h"
#include "timertask/TimerHandle.h"
#include "rpcserver/RpcServer.h"
#include "resourcemanager/ResourceManager.h"
#include "taskmanager/TaskManager.h"

void stop()
{
	//停止rpc
	SINGLETON(CMasterServer)->stopServer();

	//停止定时器
	SINGLETON(CTimerHandle)->stop();
	
	//删除所有机器注册和任务
	SINGLETON(CResourceManager)->stop();
	SINGLETON(CTaskManager)->stop();
}

void exitProcess(int sign)
{
	LOG(ERROR) << "recv exit signal(sign:" << sign << ").";
    stop();
	exit(0);
}

//注册信号
int registerSignal(int sign, void (*func)(int))
{
    struct sigaction act,oact;
    
	//传入回调函数
    act.sa_handler=func;
    
	//将act的属性sa_mask设置一个初始值
    sigemptyset(&act.sa_mask);
    act.sa_flags=0;
    return sigaction(sign, &act, &oact);
}

int main(int argc, char **argv) 
{
	try
	{
		//1.解析命令行参数（配置文件全路径）
		std::string config = "/usr/local/etc/qusprout/qusprout-master.yaml";
		const char *optstring = "c:";
		int opt = 0;
		do
		{
			opt = getopt(argc, argv, optstring);
			if (opt == 'c') {
				config = optarg;
			}
		} while(opt != -1);

		//2.解析配置文件
		int ret = SINGLETON(CConfig)->praseConfig(config);
		if (ret != 0)
		{
			return -1;
		}

		//3.初始化日志
		SINGLETON(CQuLog)->Init(SINGLETON(CConfig)->m_logNamePrefix, SINGLETON(CConfig)->m_logPath, SINGLETON(CConfig)->m_logLevel);
		LOG(INFO) << SINGLETON(CConfig)->getPrintStr();

		//4.捕捉信号，监听信号，释放资源
		//键盘Ctrl+C
		registerSignal(SIGINT, exitProcess);
		//和任何控制字符无关,用kill函数发送,与SIGKILL的不同: SIGTERM可以被阻塞,忽略,捕获,也就是说可以进行信号处理程序,那么这样就可以让进程很好的终止,允许清理和关闭文件
		registerSignal(SIGTERM, exitProcess);

		//5.启动定时器
		SINGLETON(CTimerHandle)->init();

		//6.启动服务
		std::thread thd(std::bind(&CMasterServer::startSysServer, SINGLETON(CMasterServer)));
		thd.detach(); 
		SINGLETON(CMasterServer)->startServer();
    }
    catch(const std::exception& e)
    {
        LOG(ERROR) << "main exception(err:" << e.what() << ").";
		return -2;
    }
	catch(...)
    {
        LOG(ERROR) << "main other exception.";
		return -3;
    }

	stop();
	
	return 0;
}