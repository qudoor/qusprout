#ifndef __H__WORKCLIENT__H__
#define __H__WORKCLIENT__H__

#include <string>
#include <memory>
#include <transport/TBufferTransports.h>
#include <transport/TTransportException.h>
#include <transport/TServerSocket.h>
#include "interface/WorkServer.h"
#include "interface/qusproutdata_types.h"
#include "comm/Base.h"

using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;

class CWorkClient : public CBase
{
public:
    CWorkClient();
    virtual ~CWorkClient();

    //初始化客户端
    int init(const std::string& addr, const int port);

    //关闭客户端
    void close();

    //重新初始化客户端
    int reInit(const bool isprint = true);

public:
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
    
private:
    std::string m_addr{""};
    int m_port{0};
    std::shared_ptr<WorkServerClient> m_client{nullptr};
    std::shared_ptr<TTransport> m_transport{nullptr};
};

#endif 