namespace py qusprout

include "qusproutdata.thrift"

service QuSproutServer {
    //初始化量子环境
    qusproutdata.InitQubitsResp initQubits(1:qusproutdata.InitQubitsReq req)

    //添加量子指令
    qusproutdata.SendCircuitCmdResp sendCircuitCmd(1:qusproutdata.SendCircuitCmdReq req)

    //释放量子环境
    qusproutdata.CancelCmdResp cancelCmd(1:qusproutdata.CancelCmdReq req)

    //获取振幅
    qusproutdata.GetProbAmpResp getProbAmp(1:qusproutdata.GetProbAmpReq req)

    //获取组合概率
    qusproutdata.GetProbOfAllOutcomResp getProbOfAllOutcome(1:qusproutdata.GetProbOfAllOutcomReq req)

    //获取所有的计算结果
    qusproutdata.GetAllStateResp getAllState(1:qusproutdata.GetAllStateReq req)

    //重复线路操作
    qusproutdata.RunCircuitResp run(1:qusproutdata.RunCircuitReq req)

    //获取泡利算子乘积的期望值
    qusproutdata.GetExpecPauliProdResp getExpecPauliProd(1:qusproutdata.GetExpecPauliProdReq req)

    //获取泡利算子乘积之和的期望值
    qusproutdata.GetExpecPauliSumResp getExpecPauliSum(1:qusproutdata.GetExpecPauliSumReq req)
    
    //获取测量结果
    qusproutdata.MeasureQubitsResp measureQubits(1:qusproutdata.MeasureQubitsReq req)

    //注册一些自定义量子门，单次任务有效
    qusproutdata.AddCustomGateByMatrixResp addCustomGateByMatrix(1:qusproutdata.AddCustomGateByMatrixReq req)

    //添加量子门操作
    qusproutdata.AddSubCircuitResp addSubCircuit(1:qusproutdata.AddSubCircuitReq req)

    //追加量子比特到当前的量子电路
    qusproutdata.AppendQubitsResp appendQubits(1:qusproutdata.AppendQubitsReq req)

    //重置指定的qubits
    qusproutdata.ResetQubitsResp resetQubits(1:qusproutdata.ResetQubitsReq req)

    //获取当前量子状态向量
    qusproutdata.GetStateOfAllQubitsResp getStateOfAllQubits(1:qusproutdata.GetStateOfAllQubitsReq req)

    //获取当前所有可能状态组合的概率
    qusproutdata.GetProbabilitiesResp getProbabilities(1:qusproutdata.GetProbabilitiesReq req)

    //获取任务状态
    qusproutdata.GetTaskInfoResp getTaskInfo(1:qusproutdata.GetTaskInfoReq req)

    //获取随机数卡的信息
    qusproutdata.GetRandomCardInfoResp getRandomCardInfo(1:qusproutdata.GetRandomCardInfoReq req)

    //设置随机数卡
    qusproutdata.SetRandomCardResp setRandomCard(1:qusproutdata.SetRandomCardReq req)

    //获取随机数
    qusproutdata.GetRandomResp getRandom(1:qusproutdata.GetRandomReq req)
}