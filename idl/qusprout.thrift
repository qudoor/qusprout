namespace py qusprout

include "qusproutdata.thrift"

service QuSproutServer {
    //qubit初始化
    qusproutdata.InitQubitsResp initQubits(1:qusproutdata.InitQubitsReq req)

    //设置振幅
    qusproutdata.SetAmplitudesResp setAmplitudes(1:qusproutdata.SetAmplitudesReq req)

    //发送任务
    qusproutdata.SendCircuitCmdResp sendCircuitCmd(1:qusproutdata.SendCircuitCmdReq req)

    //取消任务
    qusproutdata.CancelCmdResp cancelCmd(1:qusproutdata.CancelCmdReq req)

    //获取振幅
    qusproutdata.GetProbAmpResp getProbAmp(1:qusproutdata.GetProbAmpReq req)

    //获取当前qubit的概率
    qusproutdata.GetProbOfOutcomeResp getProbOfOutcome(1:qusproutdata.GetProbOfOutcomeReq req)

    //获取所有qubit的概率
    qusproutdata.GetProbOfAllOutcomResp getProbOfAllOutcome(1:qusproutdata.GetProbOfAllOutcomReq req)

    //获取所有的计算结果
    qusproutdata.GetAllStateResp getAllState(1:qusproutdata.GetAllStateReq req)

    //执行任务
    qusproutdata.RunCircuitResp run(1:qusproutdata.RunCircuitReq req)

    //对部分量子比特应用量子傅立叶变换
    qusproutdata.ApplyQFTResp applyQFT(1:qusproutdata.ApplyQFTReq req)

    //对所有量子比特应用量子傅立叶变换
    qusproutdata.ApplyFullQFTResp applyFullQFT(1:qusproutdata.ApplyFullQFTReq req)

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
}