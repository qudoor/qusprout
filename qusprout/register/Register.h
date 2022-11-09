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

#ifndef __H__REGISTER__H__
#define __H__REGISTER__H__

#include <string>
#include <memory>
#include <time.h>
#include <atomic>
#include <mutex>
#include "rpcclient/QuRootClient.h"
#include "common/SystemHandle.h"

class CRegister : public CBase
{
public:
    CRegister();
    virtual ~CRegister();

    //向quroot注册
    int resourceRegister();

    //处理机器注销接口
    int unResourceRegister();

    //处理机器心跳接口
    int heart();

private:
    std::mutex m_mutex;
    std::string m_resourceid{""};
};

#endif
