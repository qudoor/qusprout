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

#ifndef CMDPARAMS_H
#define CMDPARAMS_H

#include <string>

struct CmdParams
{
    int port = 9090;
    int qubits = 15;
    std::string config = "/usr/local/etc/qusprout/quwork.yaml";
    bool density = false;
    int execType = 0;
};

#endif

