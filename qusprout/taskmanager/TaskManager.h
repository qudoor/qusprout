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

#ifndef __H__TASKMANAGER__H__
#define __H__TASKMANAGER__H__

#include <map>
#include <set>
#include <mutex>
#include <memory>
#include "interface/ecode_constants.h"
#include "interface/QuSproutServer.h"
#include "interface/qusproutdata_types.h"
#include "rpcclient/QuWorkClient.h"
#include "comm/Base.h"

//任务状态
enum TaskState 
{
    //初始状态
    TASK_STATE_INITIAL = 0,

    //已初始化
    TASK_STATE_INITIALIZED = 1,

    //队列中
    TASK_STATE_QUEUED = 2,

    //执行任务中
    TASK_STATE_RUNNING = 3,

    //任务已完成
    TASK_STATE_DONE = 4,

    //任务出现异常
    TASK_STATE_ERROR = 5,

    //任务超时
    TASK_STATE_TIMEOUT = 6
};

//任务取消方式
enum CancelTaskMethod
{
    //正常取消
    CANCEL_TASK_NORMAL = 0,

    //超时取消
    CANCEL_TASK_TIMEOUT = 1,

    //异常清理取消
    CANCEL_TASK_CLEAN = 2,
};

std::string getTaskStr(const TaskState& state);

class CTask
{
public:
    //初始化对象
    int init(const InitQubitsReq& req, const int port, const pid_t childid, const ResourceData& resourcebytes);

    //qubit初始化
    void initQubits(InitQubitsResp& resp);

    //获取任务状态
    inline TaskState getTaskState() { return m_state; }

    //获取任务id
    inline std::string getTaskId() { return m_taskinfo.id; }

    //获取qubits
    inline int getQubits() { return m_taskinfo.qubits; }

    //任务处理
    void sendCircuitCmd(SendCircuitCmdResp& resp, const SendCircuitCmdReq& req);

    //取消任务
    void cancelCmd(CancelCmdResp& resp, const CancelCmdReq& req, const CancelTaskMethod method = CANCEL_TASK_NORMAL);

    //执行任务
    void run(RunCircuitResp& resp, const RunCircuitReq& req);

    //获取测量结果
    void measureQubits(MeasureQubitsResp& resp, const MeasureQubitsReq& req);

    //任务是否超时
    bool isTimeout();

    //更新超时时间
    void updateTime();

    //获取任务的执行时间
    int getEscapeTime();

    //获取任务状态字符串
    std::string getStateStr();

public:
    //创建时间
    long long int m_createtime{0};

    //更新时间
    long long int m_updatetime{0};

    //控制同一个任务多次发指令
    std::mutex m_mutex;

    //任务执行句柄
    InitQubitsReq m_taskinfo;

    //连接quwork的rpc客户端
    CQuWorkClient m_client;

    //任务状态
    TaskState m_state{TASK_STATE_INITIAL};

    //指令缓存
    std::vector<Cmd> m_cmds;    

    //是否有测量结果数据
    bool m_isexistmeasure{false};

    //测量结果
    std::vector<MeasureResult> m_results;

    //概率
    std::vector<Outcome> m_outcomes;

    //任务需要的资源大小，单位：byte
    ResourceData m_resourcebytes;

    //子进程id
    pid_t m_childid{0};
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
    
    //获取任务状态
    void getTaskInfo(GetTaskInfoResp& resp, const GetTaskInfoReq& req);
    
    //清理任务
    void cleanTask();

    //清空任务
    void cleanAllTask();

    //获取所有任务在每台机器上所占用的资源
    void getAllUseResourceBytes(ResourceData& resources);

    //开机清理task
    int killAllTask();
private:
    //查找任务
    std::shared_ptr<CTask> getTask(const std::string& id, const bool isupdatetime = true);

    //添加任务
    int addTask(const std::string& id, std::shared_ptr<CTask> task);

    //初始化进程
    int initSubProcess(const InitQubitsReq& req, pid_t& childid, int& port);

    //创建和执行子进程
    int createSubProcess(const InitQubitsReq& req, char* const* argv, pid_t& childid);

    //获取cpu执行子进程的参数
    void getParam(const InitQubitsReq& req, const int port, std::vector<std::string>& param);

    ////获取cpu执行单个子进程的参数
    void getSingleParam(const InitQubitsReq& req, const int port, std::vector<std::string>& param);

private:
    std::mutex m_mutex;
    //任务列表，key:taskid,value:CTask
    std::map<std::string, std::shared_ptr<CTask>> m_taskList;
};

#endif
