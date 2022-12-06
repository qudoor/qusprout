namespace py quroot

include "ecode.thrift"

//接口地址信息
struct AddrDetail {
    //rpc的ip或者hostname
    1: required string addr

    //rpc的port
    2: required i32 port
}

//机器信息
struct MachineDetail {
    //rpc的ip或者hostname
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
enum GpuType {
    //默认
    GpuType_Default = 0,

    //英伟达
    GpuType_Nvidia = 1
}

//机器资源信息
struct ResourceDetail {
    //总内存，单位：byte
    1: required i64 cpu_total_memory

    //当前可用内存，单位：byte
    2: required i64 cpu_free_memory

    //显存类型
    3: optional GpuType gpu_type 

    //总显存，单位：byte
    4: optional i64 gpu_total_memory

    //可用显存，单位：byte
    5: optional i64 gpu_free_memory
}

//设备信息
struct DeviceDetail {
    //机器信息
    1: required MachineDetail machine

    //机器资源信息
    2: required ResourceDetail resource
}

//tcp资源
struct TcpResourceDetail {
    //接口地址信息
    1: required AddrDetail addr_detail

    //所有资源信息
    2: required map<string, DeviceDetail> dev_list
}

//支持的门
struct GateCapacity
{
    //设备，cpu，gpu
    1: required string device

    //支持的门            
    2: required list<string> gates
}

//设备能力
struct DeviceCapacity
{
    //支持的门
    1: required list<GateCapacity> gate_cap_list
}

//量子资源注册请求
struct ResourceRegisterReq {
    //资源id，标识注册目标的唯一性
    1: required string resource_id

    //名称，目前固定"simulator"
    2: required string name

    //协议类型，0：tcp，1：rs485，3：can，4：usb，5：pice，6：bluetooth
    3: required i32 protocol

    //tcp资源
    4: required TcpResourceDetail tcp_resource

    //设备能力
    5: required DeviceCapacity cap
}

//量子资源注册响应
struct ResourceRegisterResp {
    //返回码
    1: required ecode.BaseCode base
}

//处理机器注销
struct UnResourceRegisterReq {
    //资源id，标识注册目标的唯一性
    1: required string resource_id
}

struct UnResourceRegisterResp {
    //返回码
    1: required ecode.BaseCode base
}

//处理机器心跳
struct HeartReq {
    //资源id，标识注册目标的唯一性
    1: required string resource_id

    //是否携带资源信息
    2: required bool up_resource

    //所有资源信息
    3: optional map<string, DeviceDetail> dev_list
}

struct HeartResp {
    //返回码
    1: required ecode.BaseCode base
}

service QuRootServer {
    //量子资源注册请求
    ResourceRegisterResp resourceRegister(1:ResourceRegisterReq req)

    //处理机器注销
    UnResourceRegisterResp unResourceRegister(1:UnResourceRegisterReq req)

    //处理机器心跳
    HeartResp heart(1:HeartReq req)
}