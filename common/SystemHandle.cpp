#include <iostream>
#include <errno.h>
#include <ctime>
#include <cstring>
#include <sys/utsname.h>
#include <unistd.h>
#ifdef __APPLE__
    #include <mach/vm_statistics.h>
    #include <mach/mach_types.h>
    #include <mach/mach_init.h>
    #include <mach/mach_host.h>
#else
    #include <sys/sysinfo.h>
#endif
#include "SystemHandle.h"
#include "qulog.h"

CSystemHandle::CSystemHandle()
{

}

CSystemHandle::~CSystemHandle()
{

}

//Get memory statistics
int CSystemHandle::getMemUseInfo(MemUseInfo& mem)
{
#ifdef __APPLE__
    vm_size_t pagesize;
    mach_port_t machport = mach_host_self();
    kern_return_t pageret = host_page_size(machport, &pagesize);
    if (pageret != KERN_SUCCESS)
    {
        LOG(ERROR) << "host_page_size failed(pageret:" << pageret << ").";
        return -1;
    }

    vm_statistics64_data_t vmstats;
    mach_msg_type_number_t count = sizeof(vmstats) / sizeof(natural_t);
    kern_return_t staret = host_statistics64(machport, HOST_VM_INFO, (host_info64_t)&vmstats, &count);
    if (staret != KERN_SUCCESS)
    {
        LOG(ERROR) << "host_statistics64 failed(staret:" << staret << ").";
        return -1;
    }

    long long freememory = (int64_t)vmstats.free_count * (int64_t)pagesize;
    long long usedmemory = ((int64_t)vmstats.active_count +  (int64_t)vmstats.inactive_count + (int64_t)vmstats.wire_count) *  (int64_t)pagesize;
    mem.currTime = time(NULL);
    mem.sysStartTime = 0;
    mem.totalRam = freememory + usedmemory;
    mem.freeRam = freememory;
#else
    struct sysinfo info;
    int ret = sysinfo(&info);
    if (-1 == ret)
    {
        LOG(ERROR) << "sysinfo failed(" << strerror(errno) << ").";
        return -1;
    }

    mem.currTime = time(NULL);
    mem.sysStartTime = mem.currTime - info.uptime;
    mem.totalRam = info.totalram;
    mem.freeRam = info.freeram;
#endif

    return 0;
}

//Get system statistics
int CSystemHandle::getMachineSysInfo(MachineSysInfo& sys)
{
    //Get hostname
    char hostname[513];
    size_t hostlen = sizeof(hostname);
    memset(hostname, '\0', hostlen);
    int ret = gethostname(hostname, hostlen - 1);
    if (-1 == ret)
    {
        LOG(ERROR) << "gethostname failed(" << strerror(errno) << ").";
        return -1;
    }
    sys.addr = hostname;

    //Get system info
    struct utsname sysinfo;
    ret = uname(&sysinfo);
    if (-1 == ret)
    {
        LOG(ERROR) << "uname failed(" << strerror(errno) << ").";
        return -2;
    }

    sys.sys_name = sysinfo.sysname;
    sys.sys_release = sysinfo.release;
    sys.sys_version = sysinfo.version;
    sys.sys_machine = sysinfo.machine;
    sys.create_time = time(NULL);
    return 0;
}

//Get CPU statistics
int CSystemHandle::getCpuNum()
{
#ifdef __APPLE__
    natural_t cpu_processor_count = 0;
    natural_t cpu_processor_info_count = 0;
    processor_info_array_t cpu_processor_infos = NULL;
    host_processor_info(mach_host_self(), PROCESSOR_CPU_LOAD_INFO, &cpu_processor_count, &cpu_processor_infos, &cpu_processor_info_count);
    return cpu_processor_count;
#else
    int ret = get_nprocs_conf();
    if (-1 == ret)
    {
        LOG(ERROR) << "getCpuNum failed(" << strerror(errno) << ").";
        return -1;
    }

    return ret;
#endif
}