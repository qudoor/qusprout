#include <sstream>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <thread>
#include <chrono>
#include <spawn.h>
#include <string.h>
#include "config/Config.h"
#include "common/qulog.h"
#include "common/Singleton.h"
#include "TaskManager.h"
#include "register/Register.h"

extern char **environ;

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

    if (ExecCmdType::ExecTypeDefault == req.exec_type || ExecCmdType::ExecTypeCpuSingle == req.exec_type || ExecCmdType::ExecTypeCpuMpi == req.exec_type || ExecCmdType::ExecTypeGpuSingle == req.exec_type)
    {
        initSimulator(resp, req);
        return;
    }
    
    setBase(resp.base, ErrCode::type::COM_INVALID_PARAM);
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

    std::lock_guard<std::mutex> guard(taskhandle->mutex);
    taskhandle->client.run(resp, req);
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

    std::lock_guard<std::mutex> guard(taskhandle->mutex);
    taskhandle->client.getExpecPauliSum(resp, req);
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
}

//开机清理task
int CTaskManager::killAllTask()
{
    return  killTask(SINGLETON(CConfig)->m_workBinName);
}

//定时任务清理
void CTaskManager::cleanTask()
{
    int tasknum = 0;
    CancelCmdResp resp;
    CancelCmdReq req;
    time_t now = time(NULL);

    std::lock_guard<std::mutex> guard(m_mutex);
    auto iter = m_taskList.begin();
    while (iter != m_taskList.end())
    {
        if (now - iter->second->updatetime >= SINGLETON(CConfig)->m_taskTimeOutDuration)
        {
            LOG(WARNING) << "clear task(taskid:" << iter->first << ",updatetime:" << iter->second->updatetime << ",timeOutDuration:" << SINGLETON(CConfig)->m_taskTimeOutDuration << ").";
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

//清空任务
void CTaskManager::cleanAllTask()
{
    CancelCmdResp resp;
    CancelCmdReq req;
    std::lock_guard<std::mutex> guard(m_mutex);
    auto iter = m_taskList.begin();
    while (iter != m_taskList.end())
    {
        req.__set_id(iter->second->taskinfo.id);
        std::lock_guard<std::mutex> guard(iter->second->mutex);
        iter->second->client.cancelCmd(resp, req);
    }
    m_taskList.clear();
}

//查找任务
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

//模拟器初始化
void CTaskManager::initSimulator(InitQubitsResp& resp, const InitQubitsReq& req)
{
    //1.获取一个随机端口
    int port = getLocalPort();

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
    pid_t childid = -1;
    int ret = createSubProcess(req, argv, childid);
    delete [] argv;
    if (ret != 0)
    {
        LOG(ERROR) << "createSubProcess failed(taskid:" << req.id << ",param:" << os.str() << ").";
        setBase(resp.base, ErrCode::type::COM_OTHRE);
        return;
    }
    LOG(INFO) << "createSubProcess success(taskid:" << req.id << ",param:" << os.str() << ").";

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
        ret = taskhandle->client.init("127.0.0.1", port);
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
    taskhandle->createtime = time(NULL);
    taskhandle->updatetime = taskhandle->createtime;
    taskhandle->pid = childid;
    taskhandle->taskinfo = req;

    //6.缓存客户端，调用初始化接口
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        auto iter  = m_taskList.find(req.id);
        if (iter != m_taskList.end())
        {
            //任务已经初始化
            LOG(ERROR) << "initCpuSimulator task is inited(taskid:" << req.id << ").";
            setBase(resp.base, ErrCode::type::COM_SUCCESS);
            return;
        }
        m_taskList.insert(std::pair<std::string, std::shared_ptr<CTask>>(req.id, taskhandle));
    }

    if (ExecCmdType::ExecTypeCpuMpi == req.exec_type)
    {
        setBase(resp.base, ErrCode::type::COM_SUCCESS);
    }
    else
    {
        taskhandle->client.initQubits(resp, req);
        if (resp.base.code != ErrCode::type::COM_SUCCESS)
        {
            std::lock_guard<std::mutex> guard(m_mutex);
            m_taskList.erase(req.id);
        }
    }

    return;
}

//创建和执行子进程
int CTaskManager::createSubProcess(const InitQubitsReq& req, char* const* argv, pid_t& childId)
{
    //2.创建子进程
    int ret = -1;
    childId = -1;
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

        std::string pathname = SINGLETON(CConfig)->m_workBinPath + "/" + SINGLETON(CConfig)->m_workBinName;
        if (ExecCmdType::ExecTypeCpuMpi == req.exec_type)
        {
            pathname = "/usr/local/bin/mpiexec";
        }
        ret = posix_spawn(&childId, pathname.c_str(), &fact, &attr, argv, environ);
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

    LOG(INFO) << "create sub process success(id:" << req.id << ",childpid:" << childId << ").";

    return 0;
}

//获取cpu执行子进程的参数
void CTaskManager::getParam(const InitQubitsReq& req, const int port, std::vector<std::string>& param)
{
    size_t hostsize = req.hosts.size();
    if (ExecCmdType::ExecTypeCpuMpi == req.exec_type && hostsize > 0)
    {
        std::ostringstream os("");
        //1.mpi命令
        param.push_back("/usr/local/bin/mpiexec");

        //2.执行进程数量
        param.push_back("-n");
        int prosize = getNumRanks(req.qubits, hostsize);
        os.str("");
        os << prosize;
        param.push_back(os.str());

        //3.mpi执行的机器列表
        param.push_back("-hosts");
        os.str("");
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
        param.push_back(os.str());
    }

    //4.work执行的参数
    getSingleParam(req, port, param);
}

//获取cpu执行单个子进程的参数
void CTaskManager::getSingleParam(const InitQubitsReq& req, const int port, std::vector<std::string>& param)
{
    std::ostringstream os("");
    //1.执行文件完整路径+文件名
    std::string temp = SINGLETON(CConfig)->m_workBinPath + "/" + SINGLETON(CConfig)->m_workBinName;
    param.push_back(temp);

    //2.rpc端口
    param.push_back("-p");
    os.str("");
    os << port;
    param.push_back(os.str());

    //3.配置文件完整路径+文件名
     param.push_back("-f");
    os.str("");
    os << SINGLETON(CConfig)->m_workConfigPath << "/" << SINGLETON(CConfig)->m_workConfigName;
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

//获取启动mpi的进程数
int CTaskManager::getNumRanks(const int numQubits, const int hostSize)
{
    //只有一台机器就只开一个进程
    if (1 == hostSize)
    {
        return 1;
    }

    //取小于等于并且离hostSize最近的2的n次方
    long unsigned int numranks = 1;
    long unsigned int newnumranks = 1;
    while(newnumranks <= (long unsigned int)hostSize)
    {
        numranks = newnumranks;
        newnumranks *= 2;
    }
    
    //qubit太小的时候取2的numQubits次方作为进程数
    long unsigned int numAmps = (1UL<<numQubits);
    if (numAmps < numranks)
    {
        numranks = numAmps;
    }

    return (int)numranks;
}
