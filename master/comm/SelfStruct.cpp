#include <sstream>
#include "SelfStruct.h"

std::string MachineResourceInfo::getPrintStr() const
{
    std::ostringstream os("");
    os << "Memory:" << Memory
        << ",Cpu:" << Cpu;

    return os.str();
}