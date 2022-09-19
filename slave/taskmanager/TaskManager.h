#ifndef __H__TASKMANAGER__H__
#define __H__TASKMANAGER__H__

#include <map>
#include <set>
#include <mutex>
#include <memory>
#include "interface/ecode_constants.h"
#include "interface/WorkServer.h"
#include "interface/qusproutdata_types.h"
#include "interface/statistics_types.h"
#include "rpcclient/WorkClient.h"
#include "comm/Base.h"

struct CTask
{
    //创建时间
    long createtime{0};

    //更新时间
    long updatetime{0};

    //控制同一个任务多次发指令
    std::mutex mutex;

    //任务执行句柄
    InitQubitsReq taskinfo;

    //对应work的进程id
    pid_t pid{0};

    //连接work的rpc客户端
    CWorkClient client;
};

class CTaskManager : public CBase
{
public:
    CTaskManager();
    virtual ~CTaskManager();

    //清理所有任务
    void stop();

    //qubit初始化
    void initQubits(InitQubitsResp& resp, const InitQubitsReq& req);

    //发送任务
    void sendCircuitCmd(SendCircuitCmdResp& resp, const SendCircuitCmdReq& req, InitQubitsReq& initInfo);

    //取消任务
    void cancelCmd(CancelCmdResp& resp, const CancelCmdReq& req, InitQubitsReq& initInfo);

    //获取振幅
    void getProbAmp(GetProbAmpResp& resp, const GetProbAmpReq& req, InitQubitsReq& initInfo);

    //获取当前qubit的概率
    void getProbOfOutcome(GetProbOfOutcomeResp& resp, const GetProbOfOutcomeReq& req, InitQubitsReq& initInfo);

    //获取所有qubit的概率
    void getProbOfAllOutcome(GetProbOfAllOutcomResp& resp, const GetProbOfAllOutcomReq& req, InitQubitsReq& initInfo);

    //获取所有的计算结果
    void getAllState(GetAllStateResp& resp, const GetAllStateReq& req, InitQubitsReq& initInfo);

    //执行任务
    void run(RunCircuitResp& resp, const RunCircuitReq& req, InitQubitsReq& initInfo);

    //对部分量子比特应用量子傅立叶变换
    void applyQFT(ApplyQFTResp& resp, const ApplyQFTReq& req, InitQubitsReq& initInfo);

    //对所有量子比特应用量子傅立叶变换
    void applyFullQFT(ApplyFullQFTResp& resp, const ApplyFullQFTReq& req, InitQubitsReq& initInfo);

    //获取泡利算子乘积的期望值
    void getExpecPauliProd(GetExpecPauliProdResp& resp, const GetExpecPauliProdReq& req, InitQubitsReq& initInfo);

    //获取泡利算子乘积之和的期望值
    void getExpecPauliSum(GetExpecPauliSumResp& resp, const GetExpecPauliSumReq& req, InitQubitsReq& initInfo);

    //定时清理资源
    void timerCleanTask();
    
    //开机清理task
    int killAllTask();

    //获取当前正在运行的任务信息
    void getCurrTaskInfo(std::vector<SisCurrTaskNum>& currTaskNum);

private:
    //定时任务清理
    void cleanTask(const int timeOutDuration = 0);

    //查找任务
    std::shared_ptr<CTask> getTask(const std::string& id);

    //模拟器初始化
    void initSimulator(InitQubitsResp& resp, const InitQubitsReq& req);

    //创建和执行子进程
    int createSubProcess(const InitQubitsReq& req, char* const* argv, pid_t& childId);

    //获取执行子进程的参数
    void getParam(const InitQubitsReq& req, const int port, std::vector<std::string>& param);

    //获取执行单个子进程的参数
    void getSingleParam(const InitQubitsReq& req, const int port, std::vector<std::string>& param);

    //获取启动mpi的进程数
    int getNumRanks(const int numQubits, const int hostSize);

private:
    std::mutex m_mutex;
    //任务列表，key:taskid,value:CTask
    std::map<std::string, std::shared_ptr<CTask>> m_taskList;
};

#endif
