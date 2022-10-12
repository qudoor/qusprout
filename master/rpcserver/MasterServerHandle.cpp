#include <transport/TTransportException.h>
#include "common/qulog.h"
#include "common/Singleton.h"
#include "MasterServerHandle.h"
#include "resourcemanager/ResourceManager.h"
#include "metrics/metrics.h"

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
    LOG(INFO) << "request registerResource(req:" << getPrint(req) << ").";

    SINGLETON(CResourceManager)->registerResource(resp, req);

    if (!req.resource_id.empty())
    {
        SINGLETON(CMetrics)->addResource(req.resource_id, req.device);
    }

    LOG(INFO) << "response registerResource(resp:" << getPrint(resp) << ").";
}

//处理机器注销接口
void CMasterServerHandler::unRegister(UnRegisterResp& resp, const UnRegisterReq& req)
{
    LOG(INFO) << "request unRegister(req:" << getPrint(req) << ").";

    SINGLETON(CResourceManager)->unRegister(resp, req);

    if (!req.resource_id.empty())
    {
        SINGLETON(CMetrics)->clearResource(req.resource_id);
    }

    LOG(INFO) << "response unRegister(resp:" << getPrint(resp) << ").";
}

//处理机器心跳接口
void CMasterServerHandler::heartbeat(HeartbeatResp& resp, const HeartbeatReq& req)
{
    SINGLETON(CResourceManager)->heartbeat(resp, req);

    if (!req.resource_id.empty() && req.up_resource == true)
    {
        SINGLETON(CMetrics)->addResource(req.resource_id, req.device);
    }
}
