#ifndef __H__TASKMANAGER__H__
#define __H__TASKMANAGER__H__

#include <map>
#include <set>
#include <mutex>
#include <memory>
#include "interface/ecode_constants.h"
#include "interface/QuSproutServer.h"
#include "interface/qusproutdata_types.h"
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

    //获取测量结果
    void measureQubits(MeasureQubitsResp& resp, const MeasureQubitsReq& req);

    //注册一些自定义量子门，单次任务有效
    void addCustomGateByMatrix(AddCustomGateByMatrixResp& resp, const AddCustomGateByMatrixReq& req);

    //添加量子门操作
    void addSubCircuit(AddSubCircuitResp& resp, const AddSubCircuitReq& req);

    //追加量子比特到当前的量子电路
    void appendQubits(AppendQubitsResp& resp, const AppendQubitsReq& req);

    //重置指定的qubits
    void resetQubits(ResetQubitsResp& resp, const ResetQubitsReq& req);

    //获取当前量子状态向量
    void getStateOfAllQubits(GetStateOfAllQubitsResp& resp, const GetStateOfAllQubitsReq& req);

    //获取当前所有可能状态组合的概率
    void getProbabilities(GetProbabilitiesResp& resp, const GetProbabilitiesReq& req);

    //定时清理资源
    void cleanTask();
    
    //清空任务
    void cleanAllTask();

    //开机清理task
    int killAllTask();
private:

    //查找任务
    std::shared_ptr<CTask> getTask(const std::string& id, const bool isupdatetime = true);

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
