namespace py statistics

include "ecode.thrift"
include "qusproutdata.thrift"

//基础标签
struct BaseTag {
    //主机名
    1: required string hostname

    //执行类型
    2: required qusproutdata.ExecCmdType type
}

//qubit标签
struct QubitTag {
    //基础标签
    1: required BaseTag base

    //qubit数量
    2: required i32 qubits 
}

//接口标签
struct InterfaceTag {
    //基础标签
    1: required BaseTag base

    //接口名称
    2: required string interface_name

    //qubit数量
    3: required i32 qubits 
}

//总任务数统计
struct SisAllTaskNum {
    //qubit标签
    1: required QubitTag tag

    //任务数
    2: required i32 num
}

//当前运行中的任务数统计
struct SisCurrTaskNum {
    //qubit标签
    1: required QubitTag tag

    //任务数
    2: required i32 num

    //超过1分钟没有更新的任务数
    3: required i32 timeout_num
}

//接口调用次数统计
struct SisInterfaceCount {
    //接口标签
    1: required InterfaceTag tag

    //调用次数
    2: required i32 count
}

//接口调用耗时统计
struct SisInterfaceTime {
    //接口标签
    1: required InterfaceTag tag

    //耗时范围[100ms, 200ms, 500ms, 1000ms, 2000ms, 3000ms, 5000ms, 8000ms, 10000ms, 60000ms, 120000ms, 300000ms]
    2: required i32 elapsed

    //累计次数
    3: required i32 count
}

//接口调用返回码次统计
struct SisCodeCount {
    //接口标签
    1: required InterfaceTag tag

    //返回码
    2: required i32 code

    //累计次数
    3: required i32 count
}

//显存类型
enum GpuType {
    //默认
    GpuType_Default = 0,

    //英伟达
    GpuType_Nvidia = 1
}

//机器固定资源信息
struct MachineFixedInfo {
    //cpu核心数
    1: required i32 cpu_num

    //总内存，单位：byte
    2: required i64 cpu_memory

    //显存类型
    3: optional GpuType gpu_type 

    //总显存，单位：byte
    4: optional i64 gpu_memory
}

//机器变化资源信息
struct MachineDynamicInfo {
    //已用内存，单位：byte
    1: required i64 cpu_used_memory

    //已用显存，单位：byte
    2: optional i64 gpu_used_memory
}

//统计信息
struct StatisticsInfo {
    //机器固定资源信息
    1: required MachineFixedInfo fixed_info

    //机器变化资源信息
    2: required MachineDynamicInfo dyna_info

    //当前运行中的任务数统计
    3: required list<SisCurrTaskNum> curr_task_num

    //总任务数统计
    4: required map<string, SisAllTaskNum> all_task_num

    //接口调用次数统计
    5: required map<string, SisInterfaceCount> inter_cout

    //接口调用耗时统计
    6: required map<string, SisInterfaceTime> inter_time

    //接口调用返回码次统计
    7: required map<string, SisCodeCount> code_count

    //开始时间
    8: required i64 begin_time

    //结束时间
    9: required i64 end_time
}
