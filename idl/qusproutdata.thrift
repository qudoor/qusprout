namespace py qusproutdata

include "ecode.thrift"

//执行指令的方式
enum ExecCmdType {
    ExecTypeDefault = 0,
    ExecTypeCpuSingle = 1,
    ExecTypeCpuMpi = 2,
    ExecTypeGpuSingle = 3
}

//指令数据
struct Cmd {
    //指令门类型
    1: required string gate

    //目标集
    2: required list<i32> targets

    //控制集
    3: required list<i32> controls

    //旋转集
    4: required list<double> rotation

    //指令详情描述
    5: required string desc

    //是否执行逆操作
    6: required bool inverse
}

//指令集
struct Circuit {
    //指令集
    1: required list<Cmd> cmds
}

//测量结果
struct MeasureResult {
    //量子比特
    1: required i32 id

    //测量结果
    2: required i32 value
}

//运行结果
struct Outcome {
    //比特位组成的字符串
    1: required string bitstr

    //出现次数
    2: required i32 count
}

//指令结果集
struct Result {
    //测量结果
    1: required list<MeasureResult> measureSet

    //结果集
    2: required list<Outcome> outcomeSet
}

//任务初始化信息
struct InitQubitsReq {
    //任务id
    1: required string id

    //qubit数量
    2: required i32 qubits

    //密度矩阵
    3: optional bool density

    //执行指令的方式
    4: optional ExecCmdType exec_type

    //mpi执行的host列表
    5: optional list<string> hosts

    //是否异步执行，0：否，1：是
    6: optional i32 async_type
}

struct InitQubitsResp {
    //返回码
    1: required ecode.BaseCode base
}

//发送任务
struct SendCircuitCmdReq {
    //任务id
    1: required string id

    //指令集
    2: optional Circuit circuit

    //是否释放该请求资源
    3: optional bool final
}

struct SendCircuitCmdResp {
    //返回码
    1: required ecode.BaseCode base
}

//取消任务
struct CancelCmdReq {
    //任务id
    1: required string id
}

struct CancelCmdResp {
    //返回码
    1: required ecode.BaseCode base
}

//获取振幅
struct GetProbAmpReq {
    //任务id
    1: required string id

    //qubit索引
    2: required i64 index
}

struct GetProbAmpResp {
    //返回码
    1: required ecode.BaseCode base

    //振幅
    2: optional double amp
}

//获取当前qubit的概率
struct GetProbOfOutcomeReq {
    //任务id
    1: required string id

    //qubit索引
    2: required i32 qubit

    //
    3: required i32 outcom
}

struct GetProbOfOutcomeResp {
    //返回码
    1: required ecode.BaseCode base

    //概率
    2: optional double pro_outcome
}

//获取所有qubit的概率
struct GetProbOfAllOutcomReq {
    //任务id
    1: required string id

    //概率目标列表
    2: required list<i32> targets
}

struct GetProbOfAllOutcomResp {
    //返回码
    1: required ecode.BaseCode base

    //概率
    2: optional list<double> pro_outcomes
}

//获取所有的计算状态
struct GetAllStateReq {
    //任务id
    1: required string id
}

struct GetAllStateResp {
    //返回码
    1: required ecode.BaseCode base

    //状态
    2: optional list<string> all_state
}

//对部分量子比特应用量子傅立叶变换
struct ApplyQFTReq {
    //任务id
    1: required string id

    //目标比特位
    2: required list<i32> targets
}

struct ApplyQFTResp {
    //返回码
    1: required ecode.BaseCode base
}

//对所有量子比特应用量子傅立叶变换
struct ApplyFullQFTReq {
    //任务id
    1: required string id
}

struct ApplyFullQFTResp {
    //返回码
    1: required ecode.BaseCode base
}

//执行任务
struct RunCircuitReq {
    //任务id
    1: required string id

    //运行次数
    2: required i32 shots
}

struct RunCircuitResp {
    //返回码
    1: required ecode.BaseCode base 

    //返回结果
    2: optional Result result
}

//泡利算子操作类型
enum PauliOperType {
    POT_PAULI_I = 0, 
    POT_PAULI_X = 1, 
    POT_PAULI_Y = 2, 
    POT_PAULI_Z = 3
}

//获取泡利算子乘积的期望值
struct GetExpecPauliProdReq {
    //任务id
    1: required string id

    //期望值信息
    2: required list<PauliProdInfo> pauli_prod
}

struct GetExpecPauliProdResp {
    //返回码
    1: required ecode.BaseCode base 

    //期望值
    2: optional double expect
}

struct PauliProdInfo {
    //泡利算子操作类型
    1: required PauliOperType oper_type

    //目标比特位
    2: required i32 target
}

//获取泡利算子乘积之和的期望值
struct GetExpecPauliSumReq {
    //任务id
    1: required string id

    //泡利算子操作类型，注意：oper_type_list的数量必须是qubitnum*term_coeff_list的数量
    2: required list<PauliOperType> oper_type_list

    //回归系数
    3: optional list<double> term_coeff_list
}

struct GetExpecPauliSumResp {
    //返回码
    1: required ecode.BaseCode base 

    //期望值
    2: optional double expect
}

//获取测量结果
struct MeasureQubitsReq {
    // 任务id。
    1: required string id

    // 指定要 measure 的 qubits 。
    2: required list<i32> qubits
}

struct MeasureQubitsResp {
    //返回码
    1: required ecode.BaseCode base

    //测量结果
    2: optional list<MeasureResult> results

    //结果集
    3: required list<Outcome> outcomes
}

//使用复矩阵表示的门，用于表达特殊值和优化
struct GateMatrix {
    //自定义门的名称，名称不可重复，且只对当次任务有效，可作为 Cmd.gate 的值
    1: required string name

    //门的比特数：1-3
    2: required i32 qubits

    //门的矩阵表示
    //每两个元素分别表达一个复数的实部和虚部，先行后列
    //例如，Y 门的矩阵如下：
    //   0   -i
    //   i   0
    //对应的值为 { 0,0,   0,-1,   0,1,   0,0 }
    3: required list<double> matrix
}

//注册一些自定义量子门，单次任务有效
struct AddCustomGateByMatrixReq {
    //任务id
    1: required string id

    //矩阵表示的逻辑门
    2: required GateMatrix gate
}

struct AddCustomGateByMatrixResp {
    //返回码
    1: required ecode.BaseCode base    
}

//描述一个子电路，可以作为一个自定义门被使用
struct SubCircuit {
    //自定义子电路的名称，名称不可重复，且只对当次任务有效，可作为 Cmd.gate 的值
    1: required string name

    //具体的线路定义
    2: required Circuit circuit
}

//添加量子门操作
struct AddSubCircuitReq {
    //任务id
    1: required string id

    //用量子电路表示的门
    2: required SubCircuit sub_circuit
}

struct AddSubCircuitResp {
    //返回码
    1: required ecode.BaseCode base    
}

//追加量子比特到当前的量子电路
struct AppendQubitsReq {
    //任务id
    1: required string id

    //要增加到量子电路上的量子比特数量
    2: required i32 qubits
}

struct AppendQubitsResp {
    //返回码
    1: required ecode.BaseCode base    
}

//重置指定的qubits
struct ResetQubitsReq {
    //任务id
    1: required string id

    //指定要 reset 的 qubits
    2: required list<i32> qubits
}

struct ResetQubitsResp {
    1: required ecode.BaseCode base
}

//获取当前量子状态向量
struct GetStateOfAllQubitsReq {
    //任务id
    1: required string id
}

struct GetStateOfAllQubitsResp {
    //返回码
    1: required ecode.BaseCode base

    //每 2 个元素代表一种 组合 的量子态，其中第一个元素为量子态的实部，第二个元素为量子态的虚部
    //每一种组合按照 qubit[0] qubit[1] ... qubit[n-1] 的顺序表示
    //例如，一个 2 qubits 的量子电路
    //状态向量为 [a00, a01, a10, a11] （按照组合对应的整数升序排列）
    //其中， a01 = 0.1 + 0.2i， a11 = 0.974679 
    //那么， state_vector = [ 0, 0, 0.1, 0.2, 0, 0, 0.974679, 0 ]
    2: list<double> state_vector
}

//获取当前所有可能状态组合的概率
struct GetProbabilitiesReq {
    //任务id
    1: required string id
}

struct GetProbabilitiesResp {
    //返回码
    1: required ecode.BaseCode base

    //每一个元素代表一种量子态组合的概率
    //每一种组合按照 qubit[0] qubit[1] ... qubit[n-1] 的顺序表示
    //例如，一个 2 qubits 的量子电路
    //'01' 代表 qubit[0] 为 |0> ， qubit[1] 为 |1> 的组合
    //'01' 对应的概率为 0.1 ，把 '01' 视为一个二进制数，那么 probabilities[01] == 0.1 
    //以此类推：probabilities[0] 为 '00' 的概率，probabilities[2] 为 '10' 的概率， probabilities[3] 为 '11' 的概率
    2: optional list<double> probabilities
}

//获取任务状态
struct GetTaskInfoReq {
    //任务id
    1: required string id
}

struct GetTaskInfoResp {
    //返回码
    1: required ecode.BaseCode base

    //任务状态，0：initial, 1：initialized, 2:queued, 3:running, 4:done, 5:error 
    2: required i32 state
}