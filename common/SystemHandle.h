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

#ifndef SystemHandle_H
#define SystemHandle_H

//内存信息
struct MemUseInfo 
{
    //总可用内存大小，单位：byte
    long long total_memory{0};

    //可用内存大小，单位：byte
    long long free_memory{0};

    //更新时间，单位：秒
    long update_time{0};

    //创建时间，单位：秒
    long create_time{0};
};

//机器信息
struct MachineSysInfo {
    //机器或者hostname
    std::string addr{""};

    //当前系统名称
    std::string sys_name{""};

    //当前系统发布级别
    std::string sys_release{""};

    //当前系统发布版本
    std::string sys_version{""};

    //当前硬件体系类型
    std::string sys_machine{""};

    //创建时间，单位：秒
    long create_time{0};
};

//显存类型
enum GpuFactoryType {
    //默认
    GpuFactoryType_Default = 0,

    //英伟达
    GpuFactoryType_Nvidia = 1
};

//显存信息
struct GpuUseInfo {
    //显存类型
    GpuFactoryType gpu_type{GpuFactoryType_Default};

    //总显存，单位：byte
    long long total_memory{0};

    //可用内存大小，单位：byte
    long long free_memory{0};

    //创建时间，单位：秒
    long create_time{0};
};

class CSystemHandle 
{
public:
    CSystemHandle();
    ~CSystemHandle();

    //获取内存和cpu信息
    int getMemUseInfo(MemUseInfo& mem);

    //获取系统信息
    int getMachineSysInfo(MachineSysInfo& sys);

    //获取CPU核心数
    int getCpuNum();

    //获取gpu信息
    int getGpuUseInfo(GpuUseInfo& cpu);
};

#endif
