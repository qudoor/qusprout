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

#include "testgettaskstate.h"

CTestGetTask::CTestGetTask()
{

}

CTestGetTask::~CTestGetTask()
{

}

bool CTestGetTask::run(const int qubitnum)
{
    ASSERT_RNT(qubitnum > 0);

    m_qubitnum = qubitnum;
    bool rnt = m_handle.init(m_qubitnum);
    ASSERT_RNT(rnt);

    int taskstate = 0;
    rnt = m_handle.getTask(taskstate);
    ASSERT_RNT(rnt);

    rnt = m_handle.endAdd();
    ASSERT_RNT(rnt);
    
    std::cout << "result(qubitnum:" << m_qubitnum << ",taskstate:"<< taskstate << ")." << std::endl;
    
    return true;
}
