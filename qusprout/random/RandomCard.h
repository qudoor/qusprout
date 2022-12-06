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

#ifndef __H__RANDOMCARD__H__
#define __H__RANDOMCARD__H__

#include <atomic>
#include "interface/qusproutdata_types.h"

class CRandomCardManager
{
public:
    CRandomCardManager();
    ~CRandomCardManager();

    //获取随机数卡的信息
    void getRandomCardInfo(GetRandomCardInfoResp& resp, const GetRandomCardInfoReq& req);

    //设置随机数卡
    void setRandomCard(SetRandomCardResp& resp, const SetRandomCardReq& req);

    //获取随机数
    void getRandom(GetRandomResp& resp, const GetRandomReq& req);

private:
    std::atomic_long m_index{0};
};

#endif
