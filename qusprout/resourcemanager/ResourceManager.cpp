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

#include <sstream>
#include "config/Config.h"
#include "common/qulog.h"
#include "common/Singleton.h"
#include "ResourceManager.h"
#include "interface/ecode_constants.h"
#include "taskmanager/TaskManager.h"
#include "metrics/metrics.h"

CResourceManager::CResourceManager()
{

}

CResourceManager::~CResourceManager()
{
    
}

int CResourceManager::init()
{
    //获取系统信息
    int ret = SINGLETON(CSystemHandle)->getMachineSysInfo(m_sys);
    if (ret != 0)
    {
        LOG(ERROR) << "getMachineSysInfo failed(ret:" << ret << ").";
        return -1; 
    }

    //获取内存和cpu信息
    ret = SINGLETON(CSystemHandle)->getMemUseInfo(m_mem);
    if (ret != 0)
    {
        LOG(ERROR) << "getMemUseInfo failed(ret:" << ret << ").";
        return -2;
    }

    SINGLETON(CMetrics)->addResource(m_sys, m_mem);
    return 0;
}

ErrCode::type CResourceManager::getResource(const InitQubitsReq& req, ResourceData& resourcebytes)
{
    ResourceData resources;
    SINGLETON(CTaskManager)->getAllUseResourceBytes(resources);

    if (ExecCmdType::type::ExecTypeCpuMpi == req.exec_type)
    {
        return getMpiCpuResource(req, resources, resourcebytes);
    }
    
    return getSignalCpuResource(req, resources, resourcebytes);
}

int CResourceManager::getNumRanks(const int qubits, const int hostsize)
{
    if (hostsize <= 1)
    {
        return 1;
    }

    //取小于等于并且离hostsize最近的2的n次方
    long unsigned int numranks = 1;
    long unsigned int newnumranks = 1;
    while(newnumranks <= (long unsigned int)hostsize)
    {
        numranks = newnumranks;
        newnumranks *= 2;
    }
    
    //qubit太小的时候取2的qubits次方作为进程数
    long unsigned int numamps = (1UL<<qubits);
    if (numamps < numranks)
    {
        numranks = numamps;
    }

    return (int)numranks;
}

std::string CResourceManager::getAddr()
{
    return m_sys.addr;
}

ErrCode::type CResourceManager::getMpiCpuResource(const InitQubitsReq& req, const ResourceData& useresource, ResourceData& resourcebytes)
{
    int hostsize = (int)req.hosts.size();
    int numranks = getNumRanks(req.qubits, hostsize);
    long long int mpibytes = calcMpiBytes(req.qubits, req.density, numranks);

    long long allmemory = m_mem.totalRam;
    allmemory -= useresource.cpubytes;
    if (allmemory < mpibytes)
    {
        LOG(ERROR) << "memory is not enough(taskId:" << req.id << ",allmemory:" << allmemory  << ",mpibytes:" << mpibytes << ").";
        return ErrCode::type::COM_MEM_NOT_ENOUGH;
    }

    resourcebytes.cpubytes = mpibytes;
    return ErrCode::type::COM_SUCCESS;
}

ErrCode::type CResourceManager::getSignalCpuResource(const InitQubitsReq& req, const ResourceData& useresource, ResourceData& resourcebytes)
{
    long long int singlebytes = calcSingleBytes(req.qubits, req.density);
    long long allmemory = m_mem.totalRam;
    allmemory -= useresource.cpubytes;
    if (allmemory < singlebytes)
    {
        LOG(ERROR) << "memory is not enough(taskId:" << req.id << ",allmemory:" << allmemory  << ",singlebytes:" << singlebytes << ").";
        return ErrCode::type::COM_MEM_NOT_ENOUGH;
    }

    resourcebytes.cpubytes = singlebytes;
    return ErrCode::type::COM_SUCCESS;
}

long long CResourceManager::calcSingleBytes(const int qubits, const int density)
{
    long long havebytes = 0;
    if (density)
    {
        havebytes = 16*1024*pow(2, qubits);
    }
    else
    {  
        havebytes = 16*pow(2, qubits);
    }
    return havebytes;
}

long long CResourceManager::calcMpiBytes(const int qubits, const int density, const int numranks)
{
    long long havebytes = 0;
    if (density)
    {
        havebytes = 32*1024*pow(2, qubits)/numranks;
    }
    else
    {  
        havebytes = 32*pow(2, qubits)/numranks;
    }
    return havebytes;
}

//获取所有的资源
void CResourceManager::getAllResource(std::map<std::string, DeviceDetail>& devlist)
{
    MachineDetail machine;
    machine.__set_addr(m_sys.addr);
    machine.__set_sys_name(m_sys.sys_name);
    machine.__set_sys_release(m_sys.sys_release);
    machine.__set_sys_version(m_sys.sys_version);
    machine.__set_sys_machine(m_sys.sys_machine);

    ResourceDetail resource;
    resource.__set_cpu_total_memory(m_mem.totalRam);
    resource.__set_cpu_free_memory(m_mem.freeRam);

    DeviceDetail device;
    device.__set_machine(machine);
    device.__set_resource(resource);

    devlist.insert(std::pair<std::string, DeviceDetail>(m_sys.addr, device));
}
