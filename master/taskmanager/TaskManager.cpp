#include <sstream>
#include "config/Config.h"
#include "common/qulog.h"
#include "common/Singleton.h"
#include "resourcemanager/ResourceManager.h"
#include "TaskManager.h"
#include "comm/SelfStruct.h"
#include "metrics/metrics.h"

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

int CTask::init(const InitQubitsReq& req, const RpcConnectInfo& addr, const std::string& resourceid, const ResourceData& resourcebytes)
{
    std::lock_guard<std::mutex> guard(m_mutex);
    if (addr.addr != "")
    {
        int ret = m_client.init(addr.addr, addr.port);
        if (ret != 0)
        {
            return -1;
        }
    }
    m_createtime = getCurrMs();
    m_updatetime = m_createtime;
    m_addr = addr;
    m_taskinfo = req;
    m_state = TASK_STATE_INITIAL;
    m_resourceid = resourceid;
    m_resourcebytes = resourcebytes;

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
    SINGLETON(CMetrics)->addTaskCount(getResourceId(), os.str());
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
    SINGLETON(CMetrics)->addTaskState(m_resourceid, os.str(), getTaskStr(m_state));
    SINGLETON(CMetrics)->addTaskEscapeTime(m_resourceid, os.str(), getEscapeTime());
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
    if (req.id.empty() || req.qubits <= 0)
    {
        LOG(ERROR) << "initQubits is invaild param(taskid:" << req.id << ").";
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

    RpcConnectInfo addr;
    std::string resourceid = "";
    ResourceData resourcebytes;
    std::vector<std::string> hosts;
    ErrCode::type retcode = SINGLETON(CResourceManager)->getResource(req, resourceid, addr, hosts, resourcebytes);
    if (retcode != ErrCode::type::COM_SUCCESS)
    {
        LOG(ERROR) << "initEnv resource is not enough(taskid:" << req.id << ").";
        setBase(resp.base, retcode);
        return;
    }

    InitQubitsReq reqtemp(req);
    reqtemp.__set_hosts(hosts);
    std::shared_ptr<CTask> taskhandle = std::make_shared<CTask>();
    int ret = taskhandle->init(reqtemp, addr, resourceid, resourcebytes);
    if (ret != 0)
    {
        setBase(resp.base, ErrCode::type::COM_OTHRE);
        return;
    }

    ret = addTask(req.id, taskhandle);
    if (ret != 0)
    {
        setBase(resp.base, ErrCode::type::COM_IS_EXIST);
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

void CTaskManager::cleanResourceOfTask(const std::string& resourceid)
{
    std::lock_guard<std::mutex> guard(m_mutex);
    auto iter = m_taskList.begin();
    while (iter != m_taskList.end())
    {
        if (iter->second->getResourceId() == resourceid)
        {
            CancelCmdResp resp;
            CancelCmdReq req;
            auto taskid = iter->second->getTaskId();
            req.__set_id(taskid);
            iter->second->cancelCmd(resp, req);

            LOG(INFO) << "cleanResourceOfTask(taskid:" << taskid << ",resourceid:" << resourceid << ").";

            iter = m_taskList.erase(iter);
        }
        else 
        {
            ++iter;
        }
    }
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

void CTaskManager::getAllUseResourceBytes(std::map<std::string, ResourceData>& resources)
{
    std::lock_guard<std::mutex> guard(m_mutex);
    auto iter = m_taskList.begin();
    for (; iter != m_taskList.end(); ++iter)
    {
        auto& task = iter->second;
        for (auto host : task->m_taskinfo.hosts)
        {
            resources[host] += task->m_resourcebytes;
        }
    }
}