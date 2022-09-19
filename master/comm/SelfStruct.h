#ifndef __H__SELFSTRUCT__H__
#define __H__SELFSTRUCT__H__

#include <string>

struct MachineResourceInfo
{
    //内存
    long long Memory{0};

    //cpu
    long long Cpu{0};

    std::string getPrintStr() const;
};

#endif
