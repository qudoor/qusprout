#include <getopt.h>
#include <thread>
#include <functional>
#include <signal.h>
#include <iostream>
#include <sys/wait.h>
#include <unistd.h>
#include "common/qulog.h"
#include "config/Config.h"
#include "common/Singleton.h"
#include "timertask/TimerHandle.h"
#include "rpcserver/RpcServer.h"
#include "taskmanager/TaskManager.h"
#include "register/Register.h"

//退出时释放资源
void stop()
{
	LOG(ERROR) << "begin stop.";
	
	//向master发起注销
	SINGLETON(CRegister)->stop();

	//停止定时器
	SINGLETON(CTimerHandle)->stop();

	//删除所有机器注册和任务
	SINGLETON(CTaskManager)->stop();

	//停止rpc
	SINGLETON(CSlaveServer)->stopServer();
}

//信号处理函数
void exitProcess(int sign)
{
	LOG(ERROR) << "recv exit signal(sign:" << sign << ").";
    stop();
	exit(0);
}

//子进程退出资源释放
void releaseChldProcess(int sign)
{
	int status = -1;
	pid_t pid = -1;
	while ((pid = waitpid(0, &status, WNOHANG)) > 0) 
	{
        if (WIFEXITED(status)) 
		{
            std::cerr << "process exited normal(pid:" << pid << ",status:" << WEXITSTATUS(status) << ")." << std::endl;
        } 
		else if (WIFSIGNALED(status)) 
		{
			std::cerr << "process exited exception(pid:" << pid << ",status:" << WEXITSTATUS(status) << ")." << std::endl;
        }
		else
		{
			std::cerr << "process exited exception(pid:" << pid << ")." << std::endl;
		}
    }

	return;
}

//注册处理函数
void registerToMaster()
{
	//1.向master注册
	SINGLETON(CRegister)->registerToMaster();
	//2.启动定时器
	SINGLETON(CTimerHandle)->init();
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
		std::string config = "/usr/local/etc/qusprout/qusprout-slave.yaml";
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
			std::cout << "praseConfig failed(ret:" << ret << std::endl;
			return -1;
		}

		//3.初始化日志
		SINGLETON(CQuLog)->Init(SINGLETON(CConfig)->m_logNamePrefix, SINGLETON(CConfig)->m_logPath, SINGLETON(CConfig)->m_logLevel);
		LOG(INFO) << SINGLETON(CConfig)->getPrintStr();
		LOG(INFO) << "process start(pid:" << getpid() << ").";
		
		//4.捕捉信号，监听信号，释放资源
		//键盘Ctrl+C
		registerSignal(SIGINT, exitProcess);
		//和任何控制字符无关,用kill函数发送,与SIGKILL的不同: SIGTERM可以被阻塞,忽略,捕获,也就是说可以进行信号处理程序,那么这样就可以让进程很好的终止,允许清理和关闭文件
		registerSignal(SIGTERM, exitProcess);
		//子进程结束时, 父进程会收到这个信号.
		registerSignal(SIGCHLD, releaseChldProcess);
		
		//6.清理旧的进程
		SINGLETON(CTaskManager)->killAllTask();

		//7.启动注册线程
		ret = SINGLETON(CRegister)->init();
		if (ret != 0)
		{
			LOG(ERROR) << "init failed(ret:" << ret << ").";
			return -2;
		}
		std::thread thdreg(registerToMaster);
		thdreg.detach();
		
		//8.启动服务
		SINGLETON(CSlaveServer)->startServer();
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
