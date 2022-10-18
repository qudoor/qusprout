namespace py master

include "ecode.thrift"

//接口地址信息
struct RpcConnectInfo {
    //rpc的ip或者hostname
    1: required string addr

    //rpc的port
    2: required i32 port
}

//机器信息
struct MachineInfo {
    //机器的ip或者hostname
    1: required string addr

    //当前系统名称
    2: optional string sys_name

    //当前系统发布级别
    3: optional string sys_release

    //当前系统发布版本
    4: optional string sys_version

    //当前硬件体系类型
    5: optional string sys_machine
}

//显存类型
enum GpuInfoType {
    //默认
    GpuType_Default = 0,

    //英伟达
    GpuType_Nvidia = 1
}

//机器资源信息
struct ResourceInfo {
    //总内存，单位：byte
    1: required i64 cpu_total_memory

    //当前可用内存，单位：byte
    2: required i64 cpu_free_memory

    //显存类型
    3: optional GpuInfoType gpu_type 

    //总显存，单位：byte
    4: optional i64 gpu_total_memory
}

//设备信息
struct DeviceResourceDetail {
    //机器信息
    1: required MachineInfo machine

    //机器资源信息
    2: required ResourceInfo resource
}

//处理机器注册
struct RegisterReq {
    //资源id，标识注册目标的唯一性
    1: required string resource_id

    //rpc连接信息
    2: required RpcConnectInfo rpc

    //机器资源信息
    3: required DeviceResourceDetail device
}

struct RegisterResp {
    //返回码
    1: required ecode.BaseCode base
}

//处理机器注销
struct UnRegisterReq {
    //资源id，标识注册目标的唯一性
    1: required string resource_id
}

struct UnRegisterResp {
    //返回码
    1: required ecode.BaseCode base
}

//处理机器心跳
struct HeartbeatReq {
    //资源id，标识注册目标的唯一性
    1: required string resource_id

    //是否携带资源信息
    2: required bool up_resource

    //所有资源信息
    3: optional DeviceResourceDetail device
}

struct HeartbeatResp {
    //返回码
    1: required ecode.BaseCode base
}

service MasterServer {
    //处理机器注册接口
    RegisterResp registerResource(1:RegisterReq req)

    //处理机器注销接口
    UnRegisterResp unRegister(1:UnRegisterReq req)

    //处理机器心跳接口
    HeartbeatResp heartbeat(1:HeartbeatReq req)
}