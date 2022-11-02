#include <sstream>
#include <spawn.h>
#include "config/Config.h"
#include "common/qulog.h"
#include "common/Singleton.h"
#include "resourcemanager/ResourceManager.h"
#include "TaskManager.h"
#include "comm/SelfStruct.h"
#include "metrics/metrics.h"

extern char **environ;

std::string getTaskStr(const TaskState& state)
{
    switch(state)
    {
    case TASK_STATE_INITIAL:
        return "initial";
    case TASK_STATE_INITIALIZED:
        return "initialized";
    case TASK_STATE_QUEUED:
        return "queued";
    case TASK_STATE_RUNNING:
        return "running";
    case TASK_STATE_DONE:
        return "done";
    case TASK_STATE_ERROR:
        return "error";
    case TASK_STATE_TIMEOUT:
        return "timeout";
    }
    return "";
}

int CTask::init(const InitQubitsReq& req, const int port, const pid_t childid, const ResourceData& resourcebytes)
{
    std::lock_guard<std::mutex> guard(m_mutex);
    int ret = m_client.init("127.0.0.1", port);
    if (ret != 0)
    {
        return -1;
    }

    m_createtime = getCurrMs();
    m_updatetime = m_createtime;
    m_taskinfo = req;
    m_state = TASK_STATE_INITIAL;
    m_resourcebytes = resourcebytes;
    m_childid = childid;

    return 0;
}

void CTask::initQubits(InitQubitsResp& resp)
{
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        m_client.initQubits(resp, m_taskinfo);
        if (resp.base.code != ErrCode::type::COM_SUCCESS)
        {
            m_state = TASK_STATE_ERROR;
        }
        else
        {
            m_state = TASK_STATE_INITIALIZED;
        }
    }

    std::ostringstream os("");
    os << getQubits();
    SINGLETON(CMetrics)->addTaskCount(os.str());
}

void CTask::sendCircuitCmd(SendCircuitCmdResp& resp, const SendCircuitCmdReq& req)
{
    setBase(resp.base, ErrCode::type::COM_SUCCESS);
    std::lock_guard<std::mutex> guard(m_mutex);
    m_state = TASK_STATE_RUNNING;
    m_client.sendCircuitCmd(resp, req);
    if (resp.base.code != ErrCode::type::COM_SUCCESS)
    {
        m_state = TASK_STATE_ERROR;
    }
    else if (true == req.final)
    {
        m_state = TASK_STATE_DONE;
    }
    return;
}

void CTask::cancelCmd(CancelCmdResp& resp, const CancelCmdReq& req)
{
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        setBase(resp.base, ErrCode::type::COM_SUCCESS);
        if (TASK_STATE_DONE != m_state && TASK_STATE_ERROR != m_state)
        {
            m_state = TASK_STATE_TIMEOUT;
        }
        if (m_client.isInit())
        {
            m_client.cancelCmd(resp, req);
            m_client.close();
        }
    }

    std::ostringstream os("");
    os << m_taskinfo.qubits;
    SINGLETON(CMetrics)->addTaskState(os.str(), getTaskStr(m_state));
    SINGLETON(CMetrics)->addTaskEscapeTime(os.str(), getEscapeTime());
}

void CTask::run(RunCircuitResp& resp, const RunCircuitReq& req)
{
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        m_state = TASK_STATE_RUNNING;
        m_client.run(resp, req);
        if (resp.base.code != ErrCode::type::COM_SUCCESS)
        {
            m_state = TASK_STATE_ERROR;
        }
        else
        {
            m_state = TASK_STATE_DONE;
        }
    }

    if (resp.base.code == ErrCode::type::COM_SUCCESS)
    {
        m_results = resp.result.measureSet;
        m_outcomes = resp.result.outcomeSet;
        m_isexistmeasure = true;
    }
}

void CTask::measureQubits(MeasureQubitsResp& resp, const MeasureQubitsReq& req)
{
    std::lock_guard<std::mutex> guard(m_mutex);
    if (m_isexistmeasure)
    {
        setBase(resp.base, ErrCode::type::COM_SUCCESS);
        resp.__set_results(m_results);
        resp.__set_outcomes(m_outcomes);
    }
    else
    {
        m_client.measureQubits(resp, req);
    }
}

bool CTask::isTimeout()
{
    auto now = getCurrMs();
    auto iter = SINGLETON(CConfig)->m_cleanTaskTimeout.find((int)m_state);
    if (iter != SINGLETON(CConfig)->m_cleanTaskTimeout.end())
    {
        if (now - m_updatetime > iter->second*1000)
        {
            return true;
        }
    }
    else
    {
        if (now - m_updatetime > 60 * 60 * 1000)
        {
            return true;
        }
    }

    return false;
}

void CTask::updateTime()
{
    m_updatetime = getCurrMs();
}

int CTask::getEscapeTime()
{
    return (getCurrMs() - m_createtime);
}

std::string CTask::getStateStr()
{
    return getTaskStr(m_state);
}

CTaskManager::CTaskManager()
{

}

CTaskManager::~CTaskManager()
{
    
}

void CTaskManager::initQubits(InitQubitsResp& resp, const InitQubitsReq& req)
{
    if (req.id.empty() || req.qubits <= 0 ||
        req.exec_type == ExecCmdType::type::ExecTypeCpuMpi ||
        req.exec_type == ExecCmdType::type::ExecTypeGpuSingle)
    {
        LOG(ERROR) << "initQubits is invaild param(taskid:" << req.id << ",qubits:" << req.qubits << ",exec_type:" << req.exec_type << ").";
        setBase(resp.base, ErrCode::type::COM_INVALID_PARAM);
        return;
    }

    auto tasktemp = getTask(req.id, false);
    if (tasktemp != nullptr)
    {
        LOG(ERROR) << "initQubits task is inited(taskid:" << req.id << ").";
        setBase(resp.base, ErrCode::type::COM_SUCCESS);
        return;
    }

    ResourceData resourcebytes;
    ErrCode::type retcode = SINGLETON(CResourceManager)->getResource(req, resourcebytes);
    if (retcode != ErrCode::type::COM_SUCCESS)
    {
        LOG(ERROR) << "initEnv resource is not enough(taskid:" << req.id << ").";
        setBase(resp.base, retcode);
        return;
    }

    int port = 0;
    pid_t childid = -1;
    int ret = initSubProcess(req, childid, port);
    if (ret != 0)
    {
        setBase(resp.base, ErrCode::type::COM_OTHRE);
        return;
    }

    //5.创建work的客户端,等待子线程rpc启动成功(sleeptime*10为1秒)
    const int sleeptime = 100;
    int count = SINGLETON(CConfig)->m_waitRpcTimeout*(10);
    if (ExecCmdType::ExecTypeCpuMpi == req.exec_type)
    {
        count = SINGLETON(CConfig)->m_waitMpiRpcTimeout*(10);
    }
    std::shared_ptr<CTask> taskhandle = std::make_shared<CTask>();
    while(count > 0)
    {
        ret = taskhandle->init(req, port, childid, resourcebytes);
        if (0 == ret)
        {
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(sleeptime));
        --count;
    }
    if (ret != 0)
    {
        LOG(ERROR) << "sub process start rpc failed(taskid:" << req.id << ",port:" << port<< ").";
        setBase(resp.base, ErrCode::type::COM_OTHRE);
        kill(childid, SIGKILL);
        return;
    }

    ret = addTask(req.id, taskhandle);
    if (ret != 0)
    {
        setBase(resp.base, ErrCode::type::COM_IS_EXIST);
        return;
    }

    if (ExecCmdType::ExecTypeCpuMpi == req.exec_type)
    {
        setBase(resp.base, ErrCode::type::COM_SUCCESS);
        return;
    }

    taskhandle->initQubits(resp);
}

void CTaskManager::sendCircuitCmd(SendCircuitCmdResp& resp, const SendCircuitCmdReq& req)
{
    if (req.id.empty() || (req.final == false && req.circuit.cmds.size() <= 0))
    {
        LOG(ERROR) << "sendCircuitCmd param is invalild(taskid:" << req.id << ",final:" << req.final << ").";
        setBase(resp.base, ErrCode::type::COM_INVALID_PARAM);
        return;
    } 

    //1.判断任务是否已经初始化
    auto taskhandle = getTask(req.id);
    if (taskhandle == nullptr)
    {
        LOG(ERROR) << "sendCircuitCmd task is not exist(taskid:" << req.id << ").";
        setBase(resp.base, ErrCode::type::COM_NOT_INIT);
        return;
    }

    taskhandle->sendCircuitCmd(resp, req);
}

void CTaskManager::cancelCmd(CancelCmdResp& resp, const CancelCmdReq& req)
{
    if (req.id.empty())
    {
        LOG(ERROR) << "cancelCmd taskid is null.";
        setBase(resp.base, ErrCode::type::COM_INVALID_PARAM);
        return;
    } 

    //1.判断任务是否已经初始化
    auto taskhandle = getTask(req.id);
    if (taskhandle == nullptr)
    {
        setBase(resp.base, ErrCode::type::COM_NOT_INIT);
        return;
    }

    taskhandle->cancelCmd(resp, req);

    {
        std::lock_guard<std::mutex> guard(m_mutex);
        m_taskList.erase(req.id);
    }

    return;
}

void CTaskManager::getProbAmp(GetProbAmpResp& resp, const GetProbAmpReq& req)
{
    if (req.id.empty())
    {
        LOG(ERROR) << "getProbAmp taskid is null.";
        setBase(resp.base, ErrCode::type::COM_INVALID_PARAM);
        return;
    } 

    //1.判断任务是否已经初始化
    auto taskhandle = getTask(req.id);
    if (taskhandle == nullptr)
    {
        //任务不存在
        LOG(ERROR) << "getProbAmp task is not exist(taskid:" << req.id << ").";
        setBase(resp.base, ErrCode::type::COM_NOT_INIT);
        return;
    }

    if (taskhandle->m_taskinfo.density)
    {
        LOG(ERROR) << "getProbAmp is not invaild operator of density(index:" << req.index << ").";
        setBase(resp.base, ErrCode::type::COM_INVALID_PARAM);
        return;
    }

    auto qubitnum = taskhandle->getQubits();
    auto maxindex = 1 << qubitnum;
    if (0 == qubitnum || req.index < 0 || req.index >= maxindex)
    {
        LOG(ERROR) << "getProbAmp index is invaild(qubitnum:" << qubitnum << ",index:" << req.index << ").";
        setBase(resp.base, ErrCode::type::COM_INVALID_PARAM);
        return;
    }

    std::lock_guard<std::mutex> guard(taskhandle->m_mutex);
    taskhandle->m_client.getProbAmp(resp, req);
}

void CTaskManager::getProbOfOutcome(GetProbOfOutcomeResp& resp, const GetProbOfOutcomeReq& req)
{
    if (req.id.empty())
    {
        LOG(ERROR) << "getProbOfOutcome taskid is null.";
        setBase(resp.base, ErrCode::type::COM_INVALID_PARAM);
        return;
    } 

    //1.判断任务是否已经初始化
    auto taskhandle = getTask(req.id);
    if (taskhandle == nullptr)
    {
        //任务不存在
        LOG(ERROR) << "getProbOfOutcome task is not exist(taskid:" << req.id << ").";
        setBase(resp.base, ErrCode::type::COM_NOT_INIT);
        return;
    }

    auto qubitnum = taskhandle->getQubits();
    if (req.qubit < 0 || req.qubit >= qubitnum)
    {
        LOG(ERROR) << "getProbOfOutcome qubit is invaild(qubitnum:" << qubitnum << ",qubit:" << req.qubit << ").";
        setBase(resp.base, ErrCode::type::COM_INVALID_PARAM);
        return;
    }

    std::lock_guard<std::mutex> guard(taskhandle->m_mutex);
    taskhandle->m_client.getProbOfOutcome(resp, req);
}

void CTaskManager::getProbOfAllOutcome(GetProbOfAllOutcomResp& resp, const GetProbOfAllOutcomReq& req)
{
    if (req.id.empty())
    {
        LOG(ERROR) << "getProbOfAllOutcome taskid is null.";
        setBase(resp.base, ErrCode::type::COM_INVALID_PARAM);
        return;
    } 

    //1.判断任务是否已经初始化
    auto taskhandle = getTask(req.id);
    if (taskhandle == nullptr)
    {
        //任务不存在
        LOG(ERROR) << "getProbOfAllOutcome task is not exist(taskid:" << req.id << ").";
        setBase(resp.base, ErrCode::type::COM_NOT_INIT);
        return;
    }

    auto qubitnum = taskhandle->getQubits();
    for (auto target : req.targets)
    {
        if (target < 0 || target >= qubitnum)
        {
            LOG(ERROR) << "getProbOfAllOutcome qubit is invaild(qubitnum:" << qubitnum << ",target:" << target << ").";
            setBase(resp.base, ErrCode::type::COM_INVALID_PARAM);
            return;
        }
    }

    std::lock_guard<std::mutex> guard(taskhandle->m_mutex);
    taskhandle->m_client.getProbOfAllOutcome(resp, req);
}

void CTaskManager::getAllState(GetAllStateResp& resp, const GetAllStateReq& req)
{
    if (req.id.empty())
    {
        LOG(ERROR) << "getAllState taskid is null.";
        setBase(resp.base, ErrCode::type::COM_INVALID_PARAM);
        return;
    } 

    //1.判断任务是否已经初始化
    auto taskhandle = getTask(req.id);
    if (taskhandle == nullptr)
    {
        //任务不存在
        LOG(ERROR) << "getAllState task is not exist(taskid:" << req.id << ").";
        setBase(resp.base, ErrCode::type::COM_NOT_INIT);
        return;
    }

    std::lock_guard<std::mutex> guard(taskhandle->m_mutex);
    taskhandle->m_client.getAllState(resp, req);
}

void CTaskManager::run(RunCircuitResp& resp, const RunCircuitReq& req)
{
    if (req.id.empty())
    {
        LOG(ERROR) << "run taskid is null.";
        setBase(resp.base, ErrCode::type::COM_INVALID_PARAM);
        return;
    } 

    //1.判断任务是否已经初始化
    auto taskhandle = getTask(req.id);
    if (taskhandle == nullptr)
    {
        //任务不存在
        LOG(ERROR) << "run task is not exist(taskid:" << req.id << ").";
        setBase(resp.base, ErrCode::type::COM_NOT_INIT);
        return;
    }

    taskhandle->run(resp, req);

    CancelCmdResp cancelresp;
    CancelCmdReq cancelreq;
    cancelreq.__set_id(req.id);
    cancelCmd(cancelresp, cancelreq);
}

void CTaskManager::applyQFT(ApplyQFTResp& resp, const ApplyQFTReq& req)
{
    if (req.id.empty())
    {
        LOG(ERROR) << "applyQFT taskid is null.";
        setBase(resp.base, ErrCode::type::COM_INVALID_PARAM);
        return;
    } 

    //1.判断任务是否已经初始化
    auto taskhandle = getTask(req.id);
    if (taskhandle == nullptr)
    {
        //任务不存在
        LOG(ERROR) << "applyQFT task is not exist(taskid:" << req.id << ").";
        setBase(resp.base, ErrCode::type::COM_NOT_INIT);
        return;
    }

    std::lock_guard<std::mutex> guard(taskhandle->m_mutex);
    taskhandle->m_client.applyQFT(resp, req);
}

void CTaskManager::applyFullQFT(ApplyFullQFTResp& resp, const ApplyFullQFTReq& req)
{
    if (req.id.empty())
    {
        LOG(ERROR) << "applyFullQFT taskid is null.";
        setBase(resp.base, ErrCode::type::COM_INVALID_PARAM);
        return;
    } 

    //1.判断任务是否已经初始化
    auto taskhandle = getTask(req.id);
    if (taskhandle == nullptr)
    {
        //任务不存在
        LOG(ERROR) << "applyFullQFT task is not exist(taskid:" << req.id << ").";
        setBase(resp.base, ErrCode::type::COM_NOT_INIT);
        return;
    }

    std::lock_guard<std::mutex> guard(taskhandle->m_mutex);
    taskhandle->m_client.applyFullQFT(resp, req);
}

//获取泡利算子乘积的期望值
void CTaskManager::getExpecPauliProd(GetExpecPauliProdResp& resp, const GetExpecPauliProdReq& req)
{
    if (req.id.empty())
    {
        LOG(ERROR) << "getExpecPauliProd taskid is null.";
        setBase(resp.base, ErrCode::type::COM_INVALID_PARAM);
        return;
    } 

    //1.判断任务是否已经初始化
    auto taskhandle = getTask(req.id);
    if (taskhandle == nullptr)
    {
        //任务不存在
        LOG(ERROR) << "getExpecPauliProd task is not exist(taskid:" << req.id << ").";
        setBase(resp.base, ErrCode::type::COM_NOT_INIT);
        return;
    }

    auto qubitnum = taskhandle->getQubits();
    for (auto pauli : req.pauli_prod)
    {
        if (pauli.target < 0 || pauli.target >= qubitnum)
        {
            LOG(ERROR) << "getExpecPauliProd target is invaild(qubitnum:" << qubitnum << ",target:" << pauli.target << ").";
            setBase(resp.base, ErrCode::type::COM_INVALID_PARAM);
            return;
        }
    }

    std::lock_guard<std::mutex> guard(taskhandle->m_mutex);
    taskhandle->m_client.getExpecPauliProd(resp, req);
}

//获取泡利算子乘积之和的期望值
void CTaskManager::getExpecPauliSum(GetExpecPauliSumResp& resp, const GetExpecPauliSumReq& req)
{
    if (req.id.empty())
    {
        LOG(ERROR) << "getExpecPauliSum taskid is null.";
        setBase(resp.base, ErrCode::type::COM_INVALID_PARAM);
        return;
    } 

    //1.判断任务是否已经初始化
    auto taskhandle = getTask(req.id);
    if (taskhandle == nullptr)
    {
        //任务不存在
        LOG(ERROR) << "getExpecPauliSum task is not exist(taskid:" << req.id << ").";
        setBase(resp.base, ErrCode::type::COM_NOT_INIT);
        return;
    }

    size_t typesize = req.oper_type_list.size();
    size_t coeffsize = req.term_coeff_list.size();
    if (coeffsize * taskhandle->getQubits() != typesize) 
    {
        LOG(ERROR) << "typesize is not equal of coeffsize*qubitnum.";
        setBase(resp.base, ErrCode::type::COM_INVALID_PARAM);
        return;
    }

    std::lock_guard<std::mutex> guard(taskhandle->m_mutex);
    taskhandle->m_client.getExpecPauliSum(resp, req);
}

//获取测量结果
void CTaskManager::measureQubits(MeasureQubitsResp& resp, const MeasureQubitsReq& req)
{
    if (req.id.empty())
    {
        LOG(ERROR) << "measureQubits taskid is null.";
        setBase(resp.base, ErrCode::type::COM_INVALID_PARAM);
        return;
    } 

    //1.判断任务是否已经初始化
    auto taskhandle = getTask(req.id);
    if (taskhandle == nullptr)
    {
        //任务不存在
        LOG(ERROR) << "measureQubits task is not exist(taskid:" << req.id << ").";
        setBase(resp.base, ErrCode::type::COM_NOT_INIT);
        return;
    }

    auto qubitnum = taskhandle->getQubits();
    for (auto target : req.qubits)
    {
        if (target < 0 || target >= qubitnum)
        {
            LOG(ERROR) << "measureQubits qubit is invaild(qubitnum:" << qubitnum << ",target:" << target << ").";
            setBase(resp.base, ErrCode::type::COM_INVALID_PARAM);
            return;
        }
    }

    taskhandle->measureQubits(resp, req);
}

//注册一些自定义量子门，单次任务有效
void CTaskManager::addCustomGateByMatrix(AddCustomGateByMatrixResp& resp, const AddCustomGateByMatrixReq& req)
{
    if (req.id.empty())
    {
        LOG(ERROR) << "addCustomGateByMatrix taskid is null.";
        setBase(resp.base, ErrCode::type::COM_INVALID_PARAM);
        return;
    } 

    //1.判断任务是否已经初始化
    auto taskhandle = getTask(req.id);
    if (taskhandle == nullptr)
    {
        //任务不存在
        LOG(ERROR) << "addCustomGateByMatrix task is not exist(taskid:" << req.id << ").";
        setBase(resp.base, ErrCode::type::COM_NOT_INIT);
        return;
    }

    std::lock_guard<std::mutex> guard(taskhandle->m_mutex);
    taskhandle->m_client.addCustomGateByMatrix(resp, req);
}

//添加量子门操作
void CTaskManager::addSubCircuit(AddSubCircuitResp& resp, const AddSubCircuitReq& req)
{
    if (req.id.empty())
    {
        LOG(ERROR) << "addSubCircuit taskid is null.";
        setBase(resp.base, ErrCode::type::COM_INVALID_PARAM);
        return;
    } 

    //1.判断任务是否已经初始化
    auto taskhandle = getTask(req.id);
    if (taskhandle == nullptr)
    {
        //任务不存在
        LOG(ERROR) << "addSubCircuit task is not exist(taskid:" << req.id << ").";
        setBase(resp.base, ErrCode::type::COM_NOT_INIT);
        return;
    }

    std::lock_guard<std::mutex> guard(taskhandle->m_mutex);
    taskhandle->m_client.addSubCircuit(resp, req);
}

//追加量子比特到当前的量子电路
void CTaskManager::appendQubits(AppendQubitsResp& resp, const AppendQubitsReq& req)
{
    if (req.id.empty())
    {
        LOG(ERROR) << "appendQubits taskid is null.";
        setBase(resp.base, ErrCode::type::COM_INVALID_PARAM);
        return;
    } 

    //1.判断任务是否已经初始化
    auto taskhandle = getTask(req.id);
    if (taskhandle == nullptr)
    {
        //任务不存在
        LOG(ERROR) << "appendQubits task is not exist(taskid:" << req.id << ").";
        setBase(resp.base, ErrCode::type::COM_NOT_INIT);
        return;
    }

    std::lock_guard<std::mutex> guard(taskhandle->m_mutex);
    taskhandle->m_client.appendQubits(resp, req);
}

//重置指定的qubits
void CTaskManager::resetQubits(ResetQubitsResp& resp, const ResetQubitsReq& req)
{
    if (req.id.empty())
    {
        LOG(ERROR) << "resetQubits taskid is null.";
        setBase(resp.base, ErrCode::type::COM_INVALID_PARAM);
        return;
    } 

    //1.判断任务是否已经初始化
    auto taskhandle = getTask(req.id);
    if (taskhandle == nullptr)
    {
        //任务不存在
        LOG(ERROR) << "resetQubits task is not exist(taskid:" << req.id << ").";
        setBase(resp.base, ErrCode::type::COM_NOT_INIT);
        return;
    }

    auto qubitnum = taskhandle->getQubits();
    for (auto target : req.qubits)
    {
        if (target < 0 || target >= qubitnum)
        {
            LOG(ERROR) << "resetQubits qubit is invaild(qubitnum:" << qubitnum << ",target:" << target << ").";
            setBase(resp.base, ErrCode::type::COM_INVALID_PARAM);
            return;
        }
    }

    std::lock_guard<std::mutex> guard(taskhandle->m_mutex);
    taskhandle->m_client.resetQubits(resp, req);
}

//获取当前量子状态向量
void CTaskManager::getStateOfAllQubits(GetStateOfAllQubitsResp& resp, const GetStateOfAllQubitsReq& req)
{
    if (req.id.empty())
    {
        LOG(ERROR) << "getStateOfAllQubits taskid is null.";
        setBase(resp.base, ErrCode::type::COM_INVALID_PARAM);
        return;
    } 

    //1.判断任务是否已经初始化
    auto taskhandle = getTask(req.id);
    if (taskhandle == nullptr)
    {
        //任务不存在
        LOG(ERROR) << "getStateOfAllQubits task is not exist(taskid:" << req.id << ").";
        setBase(resp.base, ErrCode::type::COM_NOT_INIT);
        return;
    }

    std::lock_guard<std::mutex> guard(taskhandle->m_mutex);
    taskhandle->m_client.getStateOfAllQubits(resp, req);
}

//获取当前所有可能状态组合的概率
void CTaskManager::getProbabilities(GetProbabilitiesResp& resp, const GetProbabilitiesReq& req)
{
    if (req.id.empty())
    {
        LOG(ERROR) << "getProbabilities taskid is null.";
        setBase(resp.base, ErrCode::type::COM_INVALID_PARAM);
        return;
    } 

    //1.判断任务是否已经初始化
    auto taskhandle = getTask(req.id);
    if (taskhandle == nullptr)
    {
        //任务不存在
        LOG(ERROR) << "getProbabilities task is not exist(taskid:" << req.id << ").";
        setBase(resp.base, ErrCode::type::COM_NOT_INIT);
        return;
    }

    std::lock_guard<std::mutex> guard(taskhandle->m_mutex);
    taskhandle->m_client.getProbabilities(resp, req);
}

//获取任务状态
void CTaskManager::getTaskInfo(GetTaskInfoResp& resp, const GetTaskInfoReq& req)
{
    if (req.id.empty())
    {
        LOG(ERROR) << "getProbabilities taskid is null.";
        setBase(resp.base, ErrCode::type::COM_INVALID_PARAM);
        return;
    } 

    //1.判断任务是否已经初始化
    auto taskhandle = getTask(req.id);
    if (taskhandle == nullptr)
    {
        //任务不存在
        LOG(ERROR) << "getProbabilities task is not exist(taskid:" << req.id << ").";
        setBase(resp.base, ErrCode::type::COM_NOT_INIT);
        return;
    }

    resp.__set_state((int)taskhandle->getTaskState());
    setBase(resp.base, ErrCode::type::COM_SUCCESS);
}

void CTaskManager::cleanTask()
{
    CTaskStateMetrics metrices;
    metrices.m_state[getTaskStr(TASK_STATE_INITIAL)] = 0;
    metrices.m_state[getTaskStr(TASK_STATE_INITIALIZED)] = 0;
    metrices.m_state[getTaskStr(TASK_STATE_QUEUED)] = 0;
    metrices.m_state[getTaskStr(TASK_STATE_RUNNING)] = 0;
    metrices.m_state[getTaskStr(TASK_STATE_DONE)] = 0;
    metrices.m_state[getTaskStr(TASK_STATE_ERROR)] = 0;
    metrices.m_state[getTaskStr(TASK_STATE_TIMEOUT)] = 0;

    {
        std::lock_guard<std::mutex> guard(m_mutex);
        auto iter = m_taskList.begin();
        while (iter != m_taskList.end())
        {
            if (iter->second->isTimeout())
            {
                CancelCmdResp resp;
                CancelCmdReq req;
                auto taskid = iter->second->getTaskId();
                req.__set_id(taskid);
                iter->second->cancelCmd(resp, req);

                metrices.m_state[iter->second->getStateStr()] += 1;
                LOG(INFO) << "cleanTask(taskid:" << taskid << ").";
                iter = m_taskList.erase(iter);
            }
            else 
            {
                metrices.m_state[iter->second->getStateStr()] += 1;
                ++iter;
            }
        }
    }

    SINGLETON(CMetrics)->addCurrTaskState(metrices);
}

void CTaskManager::cleanAllTask()
{
    CTaskStateMetrics metrices;
    metrices.m_state[getTaskStr(TASK_STATE_INITIAL)] = 0;
    metrices.m_state[getTaskStr(TASK_STATE_INITIALIZED)] = 0;
    metrices.m_state[getTaskStr(TASK_STATE_QUEUED)] = 0;
    metrices.m_state[getTaskStr(TASK_STATE_RUNNING)] = 0;
    metrices.m_state[getTaskStr(TASK_STATE_DONE)] = 0;
    metrices.m_state[getTaskStr(TASK_STATE_ERROR)] = 0;
    metrices.m_state[getTaskStr(TASK_STATE_TIMEOUT)] = 0;

    std::lock_guard<std::mutex> guard(m_mutex);
    auto iter = m_taskList.begin();
    while (iter != m_taskList.end())
    {
        CancelCmdResp resp;
        CancelCmdReq req;
        auto taskid = iter->second->getTaskId();
        req.__set_id(taskid);
        iter->second->cancelCmd(resp, req);

        LOG(INFO) << "cleanAllTask(taskid:" << taskid << ").";
    }
    m_taskList.clear();

    SINGLETON(CMetrics)->addCurrTaskState(metrices);
}

std::shared_ptr<CTask> CTaskManager::getTask(const std::string& id, const bool isupdatetime)
{
    std::lock_guard<std::mutex> guard(m_mutex);
    auto iter  = m_taskList.find(id);
    if (iter != m_taskList.end())
    {
        //每次操作重新获取更新时间,代表最近使用了，为了清理不使用的任务
        if (isupdatetime)
        {
            iter->second->updateTime();
        }
        return iter->second;
    }

    return nullptr;
}

int CTaskManager::addTask(const std::string& id, std::shared_ptr<CTask> task)
{
    std::lock_guard<std::mutex> guard(m_mutex);
    if (m_taskList.find(id) != m_taskList.end())
    {
        LOG(ERROR) << "task is inited(taskid:" << id << ").";
        return -1;
    }
    m_taskList.insert(std::pair<std::string, std::shared_ptr<CTask>>(id, task));

    return 0;
}

void CTaskManager::getAllUseResourceBytes(ResourceData& resources)
{
    std::lock_guard<std::mutex> guard(m_mutex);
    auto iter = m_taskList.begin();
    for (; iter != m_taskList.end(); ++iter)
    {
        resources += iter->second->m_resourcebytes;
    }
}

//开机清理task
int CTaskManager::killAllTask()
{
    return  killTask(SINGLETON(CConfig)->m_quworkBinName);
}

int CTaskManager::initSubProcess(const InitQubitsReq& req, pid_t& childid, int& port)
{
    //1.获取一个随机端口
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        port = getLocalPort();
    }

    //2.获取启动子进程的参数
    std::vector<std::string> param;
    getParam(req, port, param);

    //3.转换参数结构
    std::ostringstream os("");
    size_t argc = param.size();
    char** argv = new char*[argc + 1];
    size_t i = 0;
    for (; i < argc; ++i)
    {
        argv[i] = (char*)param[i].data();
        os << param[i] << " ";
    }
    argv[i] = NULL;

    //4.创建子进程
    int ret = createSubProcess(req, argv, childid);
    delete [] argv;
    if (ret != 0)
    {
        LOG(ERROR) << "createSubProcess failed(taskid:" << req.id << ",param:" << os.str() << ").";
        return -1;
    }
    LOG(INFO) << "createSubProcess success(taskid:" << req.id << ",param:" << os.str() << ").";

    return 0;
}

int CTaskManager::createSubProcess(const InitQubitsReq& req, char* const* argv, pid_t& childid)
{
    //2.创建子进程
    int ret = -1;
    childid = -1;
    sigset_t mask;
    posix_spawnattr_t attr;
    posix_spawn_file_actions_t fact;
    ret = posix_spawnattr_init(&attr);
    if (ret != 0)
    {
        LOG(ERROR) << "posix_spawnattr_init failed(id:" << req.id << ",err:" << strerror(errno) << ").";
        return -1;
    }

    bool destroyattr = false;
    bool destroyfact = false;
    do
    {
        ret = posix_spawnattr_setflags(&attr, POSIX_SPAWN_SETSIGMASK);
        if (ret != 0)
        {
            LOG(ERROR) << "posix_spawnattr_setflags failed(id:" << req.id << ",err:" << strerror(errno) << ").";
            destroyattr = true;
            break;
        }

        sigfillset(&mask);
        ret = posix_spawnattr_setsigmask(&attr, &mask);
        if (ret != 0)
        {
            LOG(ERROR) << "posix_spawnattr_setsigmask failed(id:" << req.id << ",err:" << strerror(errno) << ").";
            destroyattr = true;
            break;
        }

        ret = posix_spawn_file_actions_init(&fact);
        if (ret != 0)
        {
            LOG(ERROR) << "posix_spawnattr_setsigmask failed(id:" << req.id << ",err:" << strerror(errno) << ").";
            destroyattr = true;
            break;
        }

        std::string pathname = SINGLETON(CConfig)->m_quworkBinPath + "/" + SINGLETON(CConfig)->m_quworkBinName;
        if (ExecCmdType::ExecTypeCpuMpi == req.exec_type)
        {
            pathname = "/usr/local/bin/mpiexec";
        }
        ret = posix_spawn(&childid, pathname.c_str(), &fact, &attr, argv, environ);
        if (ret != 0)
        {
            LOG(ERROR) << "posix_spawn failed(id:" << req.id << ",pathname:" << pathname << ",err:" << strerror(errno) << ").";
            destroyattr = true;
            destroyfact = true;
            break;
        }
    } while (false);
    
    if (true == destroyattr)
    {
        ret = posix_spawnattr_destroy(&attr);
        if (ret != 0)
        {
            LOG(ERROR) << "posix_spawnattr_destroy failed(id:" << req.id << ",err:" << strerror(errno) << ").";
        }
    }

    if (true == destroyfact)
    {
        ret = posix_spawn_file_actions_destroy(&fact);
        if (ret != 0)
        {
            LOG(ERROR) << "posix_spawn_file_actions_destroy failed(id:" << req.id << ",err:" << strerror(errno) << ").";
        }
    }

    if (true == destroyattr || true == destroyfact)
    {
        return -2;
    }

    LOG(INFO) << "create sub process success(id:" << req.id << ",childpid:" << childid << ").";

    return 0;
}
//获取cpu执行子进程的参数
void CTaskManager::getParam(const InitQubitsReq& req, const int port, std::vector<std::string>& param)
{
    if (ExecCmdType::ExecTypeCpuMpi == req.exec_type)
    {
        int hostsize = (int)req.hosts.size();
        std::ostringstream os("");
        //1.mpi命令
        param.push_back("/usr/local/bin/mpiexec");

        //2.执行进程数量
        param.push_back("-n");
        int numranks = SINGLETON(CResourceManager)->getNumRanks(req.qubits, hostsize);
        os.str("");
        os << numranks;
        param.push_back(os.str());

        //3.mpi执行的机器列表
        os.str("");
        param.push_back("-hosts");
        if (0 == hostsize)
        {
            os << SINGLETON(CResourceManager)->getAddr();
        }
        else
        {
            auto iter = req.hosts.begin();
            for (; iter != req.hosts.end(); ++iter)
            {
                if (iter == req.hosts.begin())
                {
                    os << *iter;
                }
                else
                {
                    os << "," << *iter;
                }
            }
        }
        param.push_back(os.str());
    }

    //4.quwork执行的参数
    getSingleParam(req, port, param);
}

void CTaskManager::getSingleParam(const InitQubitsReq& req, const int port, std::vector<std::string>& param)
{
    std::ostringstream os("");
    //1.执行文件完整路径+文件名
    std::string temp = SINGLETON(CConfig)->m_quworkBinPath + "/" + SINGLETON(CConfig)->m_quworkBinName;
    param.push_back(temp);

    //2.rpc端口
    param.push_back("-p");
    os.str("");
    os << port;
    param.push_back(os.str());

    //3.配置文件完整路径+文件名
     param.push_back("-f");
    os.str("");
    os << SINGLETON(CConfig)->m_quworkConfigPath << "/" << SINGLETON(CConfig)->m_quworkConfigName;
    param.push_back(os.str());

    //4.qubit数量
    param.push_back("-q");
    os.str("");
    os << req.qubits;
    param.push_back(os.str());

    //5.密度矩阵
    param.push_back("-d");
    os.str("");
    os << req.density;
    param.push_back(os.str());

    //6. 执行指令的方式
    param.push_back("-m");
    os.str("");
    os << (int)req.exec_type;
    param.push_back(os.str());
}
