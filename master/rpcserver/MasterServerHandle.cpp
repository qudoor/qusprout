#include <transport/TTransportException.h>
#include "common/qulog.h"
#include "common/Singleton.h"
#include "MasterServerHandle.h"
#include "resourcemanager/ResourceManager.h"

using namespace apache::thrift::transport;

CMasterServerHandler::CMasterServerHandler()
{

}

CMasterServerHandler::~CMasterServerHandler()
{

}

//处理机器注册接口
void CMasterServerHandler::registerResource(RegisterResp& resp, const RegisterReq& req)
{
    std::string reqbuf = getPrint<RegisterReq>(req);
    LOG(INFO) << "request registerResource(req:" << reqbuf << ").";

    SINGLETON(CResourceManager)->registerResource(resp, req);

    LOG(INFO) << "response registerResource(seq:" << req.seq << ",resp:" << getPrint<RegisterResp>(resp) << ").";
}

//处理机器注销接口
void CMasterServerHandler::unRegister(UnRegisterResp& resp, const UnRegisterReq& req)
{
    std::string reqbuf = getPrint<UnRegisterReq>(req);
    LOG(INFO) << "request unRegister(req:" << reqbuf << ").";

    SINGLETON(CResourceManager)->unRegister(resp, req);

    LOG(INFO) << "response unRegister(seq:" << req.seq << ",resp:" << getPrint<UnRegisterResp>(resp) << ").";
}

//处理机器心跳接口
void CMasterServerHandler::heartbeat(HeartbeatResp& resp, const HeartbeatReq& req)
{
    SINGLETON(CResourceManager)->heartbeat(resp, req);
}

//处理上报资源接口
void CMasterServerHandler::reportResource(ReportResourceResp& resp, const ReportResourceReq& req)
{
    SINGLETON(CResourceManager)->reportResource(resp, req);
}

//上报统计信息
void CMasterServerHandler::ReportStatisticsInfo(ReportStatisticsInfoResp& resp, const ReportStatisticsInfoReq& req)
{
    std::string reqbuf = getPrint<ReportStatisticsInfoReq>(req);
    LOG(INFO) << "request ReportStatisticsInfo(req:" << reqbuf << ").";

    SINGLETON(CResourceManager)->ReportStatisticsInfo(resp, req);
}

//获取统计信息
void CMasterServerHandler::GetStatisticsInfo(GetStatisticsInfoResp& resp, const GetStatisticsInfoReq& req)
{
    SINGLETON(CResourceManager)->GetStatisticsInfo(resp, req);
}
