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

#ifndef __H__REQUESTMANAGER__H__
#define __H__REQUESTMANAGER__H__

#include "struct.h"
#include "interface/ecode_types.h"

class CReqManager 
{
public:
    CReqManager();
    ~CReqManager();

    void initEnv(const std::shared_ptr<InitEnvReq> req, std::shared_ptr<InitEnvResp> resp);
    void addCmd(const std::shared_ptr<AddGateCmdReq> req, std::shared_ptr<AddGateCmdResp> resp);
    void runCmd(const std::shared_ptr<RunGateCmdReq> req, std::shared_ptr<RunGateCmdResp> resp);
    void getAmp(const std::shared_ptr<GetAmpReq> req, std::shared_ptr<GetAmpResp> resp);
    void getProb(const std::shared_ptr<GetProbReq> req, std::shared_ptr<GetProbResp> resp);
    void getState(const std::shared_ptr<GetStateReq> req, std::shared_ptr<GetStateResp> resp);
    void releaseEnv(const std::shared_ptr<ReleaseEnvReq> req, std::shared_ptr<ReleaseEnvResp> resp);
    void getTask(const std::shared_ptr<GetTaskReq> req, std::shared_ptr<GetTaskResp> resp);
    void getMeasure(const std::shared_ptr<GetMeasureReq> req, std::shared_ptr<GetMeasureResp> resp);
    void getEPauli(const std::shared_ptr<GetEPauliReq> req, std::shared_ptr<GetEPauliResp> resp);
    void getEPauliSum(const std::shared_ptr<GetEPauliSumReq> req, std::shared_ptr<GetEPauliSumResp> resp);
    void getRcardInfo(const std::shared_ptr<GetRCardInfoReq> req, std::shared_ptr<GetRCardInfoResp> resp);
    void setRcard(const std::shared_ptr<SetRCardReq> req, std::shared_ptr<SetRCardResp> resp);
    void getRand(const std::shared_ptr<GetRandReq> req, std::shared_ptr<GetRandResp> resp);

private:
    int switchCode(const ErrCode::type code);
};

#endif
