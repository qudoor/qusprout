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

#ifndef __H__RESOURCEMANAGER__H__
#define __H__RESOURCEMANAGER__H__

#include <map>
#include <mutex>
#include <memory>
#include <string>
#include <atomic>
#include "comm/SelfStruct.h"
#include "comm/Base.h"
#include "interface/ecode_types.h"
#include "interface/qusproutdata_types.h"
#include "interface/qurootresource_types.h"
#include "common/SystemHandle.h"

class CResourceManager
{
public:
    CResourceManager();
    ~CResourceManager();

    //初始化
    int init();

    //检查资源是否足够
    ErrCode::type getResource(const InitQubitsReq& req, ResourceData& resourcebytes);

    //获取所有的资源
    void getAllResource(std::map<std::string, DeviceDetail>& devlist);

    //获取启动mpi的进程数
    int getNumRanks(const int qubits, const int hostsize);
    
    //获取地址
    std::string getAddr();

private:
    //获取mpi多个cpu资源的信息
    ErrCode::type getMpiCpuResource(const InitQubitsReq& req, const ResourceData& useresource, ResourceData& resourcebytes);

    //获取单个cpu资源的信息
    ErrCode::type getSignalCpuResource(const InitQubitsReq& req, const ResourceData& useresource, ResourceData& resourcebytes);

    //计算单机需要的内存
    long long calcSingleBytes(const int qubits, const int density);

    //计算mpi需要的内存
    long long calcMpiBytes(const int qubits, const int density, const int numranks);

private:
    MachineSysInfo m_sys;
    MemUseInfo m_mem;
};

#endif
