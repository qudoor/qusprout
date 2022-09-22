#include <sstream>
#include "config/Config.h"
#include "common/qulog.h"
#include "common/Singleton.h"
#include "resourcemanager/ResourceManager.h"
#include "TaskManager.h"
#include "comm/SelfStruct.h"

CTaskManager::CTaskManager()
{

}

CTaskManager::~CTaskManager()
{
    
}

//清理所有任务
void CTaskManager::stop()
{
    LOG(INFO) << "begin curr machine task is exit.";
    cleanTask(0);
    LOG(INFO) << "curr machine task is exited.";
}

void CTaskManager::initTask(InitQubitsResp& resp, const InitQubitsReq& req, const std::string& addr, const std::string& rpcAddr, const int& rpcPort, const std::vector<std::string>& hosts)
{
    std::shared_ptr<CTask> taskhandle = std::make_shared<CTask>();
    int ret = taskhandle->client.init(rpcAddr, rpcPort);
    if (ret != 0)
    {
        setBase(resp.base, ErrCode::type::COM_OTHRE);
        return;
    }
    taskhandle->createtime = time(NULL);
    taskhandle->updatetime = taskhandle->createtime;
    taskhandle->addr = addr;
    taskhandle->taskinfo = req;
    if (taskhandle->taskinfo.hosts.size() == 0)
    {
        taskhandle->taskinfo.__set_hosts(hosts);
    }

    ret = addTask(req.id, taskhandle);
    if (ret != 0)
    {
        setBase(resp.base, ErrCode::type::COM_SUCCESS);
        return;
    }
    
    {
        std::lock_guard<std::mutex> guard(taskhandle->mutex);
        taskhandle->client.initQubits(resp, taskhandle->taskinfo);
        if (resp.base.code == ErrCode::type::COM_SUCCESS)
        {
            return;
        }
    }

    {
        std::lock_guard<std::mutex> guard(m_mutex);
        m_taskList.erase(req.id);
    }
}

void CTaskManager::initCpuSimulator(InitQubitsResp& resp, const InitQubitsReq& req)
{
    //1.定义判断内存是否足够的函数
    std::string addr = "";
    std::string rpcaddr = "";
    int rpcport = 0;
    std::vector<std::string> hosts;
    ErrCode::type code = SINGLETON(CResourceManager)->checkCpuResource(req, addr, rpcaddr, rpcport, hosts);
    if (code != ErrCode::type::COM_SUCCESS)
    {
        //资源申请出错
        LOG(ERROR) << "initCpuSimulator checkCpuResource is not success(taskid:" << req.id << ",code:" << code << ").";
        setBase(resp.base, code);
        return;
    }

    //2.创建slaver的rpc客户端
    initTask(resp, req, addr, rpcaddr, rpcport, hosts);
    return;
}

void CTaskManager::initGpuSimulator(InitQubitsResp& resp, const InitQubitsReq& req)
{
    //1.定义判断内存是否足够的函数
    std::string addr = "";
    std::string rpcaddr = "";
    int rpcport = 0;
    std::vector<std::string> hosts;
    ErrCode::type code = SINGLETON(CResourceManager)->checkGpuResource(req, addr, rpcaddr, rpcport, hosts);
    if (code != ErrCode::type::COM_SUCCESS)
    {
        //资源申请出错
        LOG(ERROR) << "initGpuSimulator checkGpuResource is not success(taskid:" << req.id << ",code:" << code << ").";
        setBase(resp.base, code);
        return;
    }

    //2.创建slaver的rpc客户端
    initTask(resp, req, addr, rpcaddr, rpcport, hosts);
    return;
}

void CTaskManager::initFixedSimulator(InitQubitsResp& resp, const InitQubitsReq& req)
{
    //1.定义判断内存是否足够的函数
    std::string addr = "";
    std::string rpcaddr = "";
    int rpcport = 0;
    ErrCode::type code = SINGLETON(CResourceManager)->checkFixedResource(req, req.hosts, addr, rpcaddr, rpcport);
    if (code != ErrCode::type::COM_SUCCESS)
    {
        //资源申请出错
        LOG(ERROR) << "initFixedSimulator checkFixedResource is not success(taskid:" << req.id << ",code:" << code << ").";
        setBase(resp.base, code);
        return;
    }

    //2.创建slaver的rpc客户端
    initTask(resp, req, addr, rpcaddr, rpcport, req.hosts);
    return;
}

void CTaskManager::initQubits(InitQubitsResp& resp, const InitQubitsReq& req)
{
    if (req.id.empty() || req.qubits <= 0)
    {
        LOG(ERROR) << "initQubits is invaild param.";
        setBase(resp.base, ErrCode::type::COM_INVALID_PARAM);
        return;
    }

    //1.判断任务是否已经初始化
    auto taskhandle = getTask(req.id, false);
    if (taskhandle != nullptr)
    {
        //任务已经初始化
        LOG(ERROR) << "initQubits task is inited(taskid:" << req.id << ").";
        setBase(resp.base, ErrCode::type::COM_SUCCESS);
        return;
    }

    if (req.hosts.size() > 0)
    {
        initFixedSimulator(resp, req);
        return;
    }
    
    if (ExecCmdType::ExecTypeGpuSingle == req.exec_type)
    {
        initGpuSimulator(resp, req);
        return;
    }
    
    initCpuSimulator(resp, req);
}

void CTaskManager::setAmplitudes(SetAmplitudesResp& resp, const SetAmplitudesReq& req)
{
    if (req.id.empty())
    {
        LOG(ERROR) << "setAmplitudes is invaild param.";
        setBase(resp.base, ErrCode::type::COM_INVALID_PARAM);
        return;
    }

    //1.判断任务是否已经初始化
    auto taskhandle = getTask(req.id);
    if (taskhandle == nullptr)
    {
        //任务不存在
        LOG(ERROR) << "setAmplitudes task is not exist(taskid:" << req.id << ").";
        setBase(resp.base, ErrCode::type::QUROOT_NOT_INIT);
        return;
    }

    std::lock_guard<std::mutex> guard(taskhandle->mutex);
    taskhandle->client.setAmplitudes(resp, req);
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
        //任务不存在
        LOG(ERROR) << "sendCircuitCmd task is not exist(taskid:" << req.id << ").";
        setBase(resp.base, ErrCode::type::QUROOT_NOT_INIT);
        return;
    }

    std::lock_guard<std::mutex> guard(taskhandle->mutex);
    taskhandle->client.sendCircuitCmd(resp, req);

    return;
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
        //任务不存在
        LOG(ERROR) << "cancelCmd task is not exist(taskid:" << req.id << ").";
        setBase(resp.base, ErrCode::type::QUROOT_NOT_INIT);
        return;
    }

    {
        std::lock_guard<std::mutex> guard(taskhandle->mutex);
        taskhandle->client.cancelCmd(resp, req);
    }

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
        setBase(resp.base, ErrCode::type::QUROOT_NOT_INIT);
        return;
    }

    std::lock_guard<std::mutex> guard(taskhandle->mutex);
    taskhandle->client.getProbAmp(resp, req);

    return;
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
        setBase(resp.base, ErrCode::type::QUROOT_NOT_INIT);
        return;
    }

    std::lock_guard<std::mutex> guard(taskhandle->mutex);
    taskhandle->client.getProbOfOutcome(resp, req);

    return;
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
        setBase(resp.base, ErrCode::type::QUROOT_NOT_INIT);
        return;
    }

    std::lock_guard<std::mutex> guard(taskhandle->mutex);
    taskhandle->client.getProbOfAllOutcome(resp, req);

    return;
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
        setBase(resp.base, ErrCode::type::QUROOT_NOT_INIT);
        return;
    }

    std::lock_guard<std::mutex> guard(taskhandle->mutex);
    taskhandle->client.getAllState(resp, req);

    return;
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
        setBase(resp.base, ErrCode::type::QUROOT_NOT_INIT);
        return;
    }

    {
        std::lock_guard<std::mutex> guard(taskhandle->mutex);
        taskhandle->client.run(resp, req);
    }

    {
        std::lock_guard<std::mutex> guard(m_mutex);
        m_taskList.erase(req.id);
    }

    return;
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
        setBase(resp.base, ErrCode::type::QUROOT_NOT_INIT);
        return;
    }

    std::lock_guard<std::mutex> guard(taskhandle->mutex);
    taskhandle->client.applyQFT(resp, req);

    return;
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
        setBase(resp.base, ErrCode::type::QUROOT_NOT_INIT);
        return;
    }

    std::lock_guard<std::mutex> guard(taskhandle->mutex);
    taskhandle->client.applyFullQFT(resp, req);

    return;
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
        setBase(resp.base, ErrCode::type::QUROOT_NOT_INIT);
        return;
    }

    std::lock_guard<std::mutex> guard(taskhandle->mutex);
    taskhandle->client.getExpecPauliProd(resp, req);

    return;
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
        setBase(resp.base, ErrCode::type::QUROOT_NOT_INIT);
        return;
    }

    size_t typesize = req.oper_type_list.size();
    size_t coeffsize = req.term_coeff_list.size();
    if (coeffsize * taskhandle->taskinfo.qubits != typesize) 
    {
        LOG(ERROR) << "typesize is not equal of coeffsize*qubitnum.";
        setBase(resp.base, ErrCode::type::COM_INVALID_PARAM);
        return;
    }

    std::lock_guard<std::mutex> guard(taskhandle->mutex);
    taskhandle->client.getExpecPauliSum(resp, req);

    return;
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
        setBase(resp.base, ErrCode::type::QUROOT_NOT_INIT);
        return;
    }

    std::lock_guard<std::mutex> guard(taskhandle->mutex);
    taskhandle->client.measureQubits(resp, req);

    return;
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
        setBase(resp.base, ErrCode::type::QUROOT_NOT_INIT);
        return;
    }

    std::lock_guard<std::mutex> guard(taskhandle->mutex);
    taskhandle->client.addCustomGateByMatrix(resp, req);

    return;
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
        setBase(resp.base, ErrCode::type::QUROOT_NOT_INIT);
        return;
    }

    std::lock_guard<std::mutex> guard(taskhandle->mutex);
    taskhandle->client.addSubCircuit(resp, req);

    return;
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
        setBase(resp.base, ErrCode::type::QUROOT_NOT_INIT);
        return;
    }

    std::lock_guard<std::mutex> guard(taskhandle->mutex);
    taskhandle->client.appendQubits(resp, req);

    return;
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
        setBase(resp.base, ErrCode::type::QUROOT_NOT_INIT);
        return;
    }

    std::lock_guard<std::mutex> guard(taskhandle->mutex);
    taskhandle->client.resetQubits(resp, req);

    return;
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
        setBase(resp.base, ErrCode::type::QUROOT_NOT_INIT);
        return;
    }

    std::lock_guard<std::mutex> guard(taskhandle->mutex);
    taskhandle->client.getStateOfAllQubits(resp, req);

    return;
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
        setBase(resp.base, ErrCode::type::QUROOT_NOT_INIT);
        return;
    }

    std::lock_guard<std::mutex> guard(taskhandle->mutex);
    taskhandle->client.getProbabilities(resp, req);

    return;
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
            iter->second->updatetime = time(NULL);
        }
        return iter->second;
    }

    return nullptr;
}

void CTaskManager::timerCleanTask()
{
    cleanTask(SINGLETON(CConfig)->m_taskTimeOutDuration);
}

void CTaskManager::cleanTask(const int timeOutDuration)
{
    int tasknum = 0;
    CancelCmdResp resp;
    CancelCmdReq req;
    time_t now = time(NULL);

    std::lock_guard<std::mutex> guard(m_mutex);
    auto iter = m_taskList.begin();
    while (iter != m_taskList.end())
    {
        if (now - iter->second->updatetime >= timeOutDuration)
        {
            LOG(WARNING) << "clear task(taskid:" << iter->first << ",updatetime:" << iter->second->updatetime << ",timeOutDuration:" << timeOutDuration << ").";
            req.__set_id(iter->second->taskinfo.id);
            {
                std::lock_guard<std::mutex> guard(iter->second->mutex);
                iter->second->client.cancelCmd(resp, req);
            }
            iter = m_taskList.erase(iter);
        }
        else
        {
            iter++;
            ++tasknum;
        }
    }

    LOG(INFO) << "curr task list(tasknum:" << tasknum << ").";
}

void CTaskManager::cleanTask(const std::string& addr)
{
    CancelCmdResp resp;
    CancelCmdReq req;
    std::lock_guard<std::mutex> guard(m_mutex);
    auto iter = m_taskList.begin();
    while (iter !=  m_taskList.end())
    {
        if (iter->second->addr == addr)
        {
            std::lock_guard<std::mutex> guard(iter->second->mutex);
            req.__set_id(iter->second->taskinfo.id);
            iter->second->client.cancelCmd(resp, req);
            iter = m_taskList.erase(iter);
        }
        else
        {
            ++iter;
        }
    }
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
