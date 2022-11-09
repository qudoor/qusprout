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
#include "SelfStruct.h"

std::string MachineResourceInfo::getPrintStr() const
{
    std::ostringstream os("");
    os << "Memory:" << Memory
        << ",Cpu:" << Cpu;

    return os.str();
}