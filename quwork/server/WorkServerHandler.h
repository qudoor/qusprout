#ifndef WorkServerHandler_H
#define WorkServerHandler_H

#include <thrift/server/TThreadedServer.h>
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TThreadPoolServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TBufferTransports.h>
#include <thrift/processor/TMultiplexedProcessor.h>
#include <thrift/concurrency/ThreadFactory.h>
#include <thrift/concurrency/ThreadManager.h>

#include "../interface/QuSproutServer.h"
#include "../command/executor.h"
#include "../node/WorkNodeHandler.h"

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::server;
using namespace ::apache::thrift::concurrency;

class CWorkServerHandler : virtual public QuSproutServerIf 
{
public:
    CWorkServerHandler();
    virtual ~CWorkServerHandler();

    //qubit初始化
    void initQubits(InitQubitsResp& resp, const InitQubitsReq& req);

    //发送任务
    void sendCircuitCmd(SendCircuitCmdResp& resp, const SendCircuitCmdReq& req);

    //执行任务
    void run(RunCircuitResp& resp, const RunCircuitReq& req);

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

    //设置thread server
    void SetThreadedServer(std::shared_ptr<TThreadedServer>);
    
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

private:
    QS::CmdExecutor m_executor;
    
    std::shared_ptr<TThreadedServer> m_server;

    CWorkNodeHandler workNodeHandler;

    void setBase(BaseCode& base, const ErrCode::type& code);
};

#endif