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

#ifndef __H__QUSPROUTSERVERHANDLE__H__
#define __H__QUSPROUTSERVERHANDLE__H__

#include "interface/QuSproutServer.h"
#include "interface/qusproutdata_types.h"
#include "comm/Base.h"

class CQuSproutServerHandler : virtual public QuSproutServerIf, public CBase 
{
public:
    CQuSproutServerHandler();
    virtual ~CQuSproutServerHandler();

    //qubit初始化
    void initQubits(InitQubitsResp& resp, const InitQubitsReq& req);

    //发送任务
    void sendCircuitCmd(SendCircuitCmdResp& resp, const SendCircuitCmdReq& req);

    //取消任务
    void cancelCmd(CancelCmdResp& resp, const CancelCmdReq& req);

    //获取单个计算基态的概率
    void getProbAmp(GetProbAmpResp& resp, const GetProbAmpReq& req);

    //获取所有计算基态的概率
    void getProbOfAllOutcome(GetProbOfAllOutcomResp& resp, const GetProbOfAllOutcomReq& req);

    //获取所有计算基态的振幅
    void getAllState(GetAllStateResp& resp, const GetAllStateReq& req);

    //执行任务
    void run(RunCircuitResp& resp, const RunCircuitReq& req);

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

    //获取随机数卡的信息
    void getRandomCardInfo(GetRandomCardInfoResp& resp, const GetRandomCardInfoReq& req);

    //设置随机数卡
    void setRandomCard(SetRandomCardResp& resp, const SetRandomCardReq& req);

    //获取随机数
    void getRandom(GetRandomResp& resp, const GetRandomReq& req);
};

#endif