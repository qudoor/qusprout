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

#ifndef QuESTIniter_H
#define QuESTIniter_H

#include "QuEST.h"

class CQuESTIniter
{
public:
    CQuESTIniter();
    virtual ~CQuESTIniter();

    void Init(const int qubits, const bool density);
    void finalize();

    void createWorkQureg(Qureg& workqureg);
    void destroyWorkQureg(Qureg& workqureg);
    
    void reInit(const int qubits);

public:
    QuESTEnv        m_env;
    Qureg           m_qureg;
    int             m_qubits{0};
    bool            m_isInit{false};
    bool            m_density{0};
};

#endif

