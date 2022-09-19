namespace py master

include "ecode.thrift"
include "statistics.thrift"

//机器资源信息
struct ResourceInfo {
    //总cpu，1个核代表1000，4个核就是4*1000
    1: required i64 total_cpu

    //当前可用cpu，1个核代表1000，4个核就是4*1000
    2: required i64 free_cpu

    //总内存，单位：byte
    3: required i64 total_memory

    //当前可用内存，单位：byte
    4: required i64 free_memory

    //获取资源的时间戳，单位：秒
    5: required i64 create_time
}

//机器信息
struct MachineInfo {
    //ip或者hostname
    1: required string addr

    //当前系统名称
    2: optional string sys_name

    //当前系统发布级别
    3: optional string sys_release

    //当前系统发布版本
    4: optional string sys_version

    //当前硬件体系类型
    5: optional string sys_machine

    //注册时间戳，单位：秒
    6: required i64 create_time
}

//rpc连接信息
struct RpcConnectInfo {
    //rpc的ip或者hostname
    1: required string addr

    //rpc的port
    2: required i32 port
}

//机器类型
enum RegType {
    //cpu模拟器
    RegType_Cpu_Simulator = 0,

    //gpu模拟器
    RegType_Gpu_Simulator = 1
}

//处理机器注册
struct RegisterReq {
    //机器类型
    1: required RegType type

    //流水号
    2: required string seq

    //机器信息
    3: required MachineInfo machine

    //rpc连接信息
    4: required RpcConnectInfo rpc

    //机器资源信息
    5: required ResourceInfo resource
}

struct RegisterResp {
    //返回码
    1: required ecode.BaseCode base
}

//处理机器注销
struct UnRegisterReq {
    //机器类型
    1: required RegType type

    //流水号
    2: required string seq

    //机器信息
    3: required MachineInfo machine
}

struct UnRegisterResp {
    //返回码
    1: required ecode.BaseCode base
}

//处理机器心跳
struct HeartbeatReq {
    //机器类型
    1: required RegType type

    //流水号
    2: required string seq

    //机器信息
    3: required MachineInfo machine
}

struct HeartbeatResp {
    //返回码
    1: required ecode.BaseCode base
}

//处理上报资源
struct ReportResourceReq {
    //机器类型
    1: required RegType type

    //流水号
    2: required string seq

    //机器信息
    3: required MachineInfo machine

    //机器资源信息
    4: required ResourceInfo resource
}

struct ReportResourceResp {
    //返回码
    1: required ecode.BaseCode base
}

//上报统计信息
struct ReportStatisticsInfoReq {
    //流水号
    1: required string seq

    //机器信息
    2: required MachineInfo machine

    //机器类型
    3: required statistics.StatisticsInfo sis
}

struct ReportStatisticsInfoResp {
    //返回码
    1: required ecode.BaseCode base
}

//获取统计信息
struct GetStatisticsInfoReq {
    //流水号
    1: required string seq
}

struct GetStatisticsInfoResp {
    //返回码
    1: required ecode.BaseCode base

    //统计信息
    2: required map<string, statistics.StatisticsInfo> sis_list
}

service MasterServer {
    //处理机器注册接口
    RegisterResp registerResource(1:RegisterReq req)

    //处理机器注销接口
    UnRegisterResp unRegister(1:UnRegisterReq req)

    //处理机器心跳接口
    HeartbeatResp heartbeat(1:HeartbeatReq req)

    //处理上报资源接口
    ReportResourceResp reportResource(1:ReportResourceReq req)

    //上报统计信息
    ReportStatisticsInfoResp ReportStatisticsInfo(1:ReportStatisticsInfoReq req)

    //获取统计信息
    GetStatisticsInfoResp GetStatisticsInfo(1:GetStatisticsInfoReq req)
}