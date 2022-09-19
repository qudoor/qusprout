#ifndef __H__MASTERSERVERHANDLE__H__
#define __H__MASTERSERVERHANDLE__H__

#include "interface/MasterServer.h"
#include "interface/qusproutdata_types.h"
#include "comm/Base.h"

class CMasterServerHandler : virtual public MasterServerIf, public CBase  
{
public:
    CMasterServerHandler();
    virtual ~CMasterServerHandler();

    //处理机器注册接口
    void registerResource(RegisterResp& resp, const RegisterReq& req);

    //处理机器注销接口
    void unRegister(UnRegisterResp& resp, const UnRegisterReq& req);

    //处理机器心跳接口
    void heartbeat(HeartbeatResp& resp, const HeartbeatReq& req);

    //处理上报资源接口
    void reportResource(ReportResourceResp& resp, const ReportResourceReq& req);

    //上报统计信息
    void ReportStatisticsInfo(ReportStatisticsInfoResp& resp, const ReportStatisticsInfoReq& req);

    //获取统计信息
    void GetStatisticsInfo(GetStatisticsInfoResp& resp, const GetStatisticsInfoReq& req);
};

#endif