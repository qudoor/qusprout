#ifndef __H__TASKMANAGER__H__
#define __H__TASKMANAGER__H__

#include <map>
#include <set>
#include <mutex>
#include <memory>
#include "interface/ecode_constants.h"
#include "interface/QuSproutServer.h"
#include "interface/qusproutdata_types.h"
#include "rpcclient/SlaveClient.h"
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

    //对应机器ip或者地址
    std::string addr{0};

    //连接slaver的rpc客户端
    CSlaveClient client;
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
    void sendCircuitCmd(SendCircuitCmdResp& resp, const SendCircuitCmdReq& req);

    //取消任务
    void cancelCmd(CancelCmdResp& resp, const CancelCmdReq& req);

    //获取振幅
    void getProbAmp(GetProbAmpResp& resp, const GetProbAmpReq& req);

    //获取当前qubit的概率
    void getProbOfOutcome(GetProbOfOutcomeResp& resp, const GetProbOfOutcomeReq& req);

    //获取所有qubit的概率
    void getProbOfAllOutcome(GetProbOfAllOutcomResp& resp, const GetProbOfAllOutcomReq& req);

    //获取所有的计算结果
    void getAllState(GetAllStateResp& resp, const GetAllStateReq& req);

    //执行任务
    void run(RunCircuitResp& resp, const RunCircuitReq& req);

    //对部分量子比特应用量子傅立叶变换
    void applyQFT(ApplyQFTResp& resp, const ApplyQFTReq& req);

    //对所有量子比特应用量子傅立叶变换
    void applyFullQFT(ApplyFullQFTResp& resp, const ApplyFullQFTReq& req);
    
    //获取泡利算子乘积的期望值
    void getExpecPauliProd(GetExpecPauliProdResp& resp, const GetExpecPauliProdReq& req);

    //获取泡利算子乘积之和的期望值
    void getExpecPauliSum(GetExpecPauliSumResp& resp, const GetExpecPauliSumReq& req);

    //定时清理资源
    void timerCleanTask();

    //清理某个机器的所有任务清理
    void cleanTask(const std::string& addr);
private:
    //定时任务清理
    void cleanTask(const int timeOutDuration = 0);

    //查找任务
    std::shared_ptr<CTask> getTask(const std::string& id);

    //task创建和初始化
    void initTask(InitQubitsResp& resp, const InitQubitsReq& req, const std::string& addr, const std::string& rpcAddr, const int& rpcPort, const std::vector<std::string>& hosts);

    //cpu模拟器初始化
    void initCpuSimulator(InitQubitsResp& resp, const InitQubitsReq& req);

    //Gpu模拟器初始化
    void initGpuSimulator(InitQubitsResp& resp, const InitQubitsReq& req);

private:
    std::mutex m_mutex;
    //任务列表，key:taskid,value:CTask
    std::map<std::string, std::shared_ptr<CTask>> m_taskList;
    //机器和任务对应关系，key:addr, taskid
    std::map<std::string, std::set<std::string>> m_addrList;
};

#endif
