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

struct MemUseInfo 
{
    //系统启动时间，单位：秒
    long sysStartTime{0};

    //接口调用时间，单位：秒
    long currTime{0};

    //总可用内存大小，单位：byte
    unsigned long totalRam{0};

    //可用内存大小，单位：byte
    unsigned long freeRam{0};
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

    //注册时间戳，单位：秒
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
};

#endif
