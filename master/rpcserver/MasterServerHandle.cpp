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

    try
    {
       SINGLETON(CResourceManager)->registerResource(resp, req);
    }
    catch(const TTransportException& e)
    {
        setBase(resp.base, ErrCode::type::COM_EXCEPTION);
        LOG(ERROR) << "registerResource exception(req:" << reqbuf << ",err:" << e.what() << ").";
    }
    catch(...)
    {
        setBase(resp.base, ErrCode::type::COM_EXCEPTION);
        LOG(ERROR) << "registerResource other exception(req:" << reqbuf << ").";
    }
    
    LOG(INFO) << "response registerResource(seq:" << req.seq << ",resp:" << getPrint<RegisterResp>(resp) << ").";
}

//处理机器注销接口
void CMasterServerHandler::unRegister(UnRegisterResp& resp, const UnRegisterReq& req)
{
    std::string reqbuf = getPrint<UnRegisterReq>(req);
    LOG(INFO) << "request unRegister(req:" << reqbuf << ").";

    try
    {
        SINGLETON(CResourceManager)->unRegister(resp, req);
    }
    catch(const TTransportException& e)
    {
        setBase(resp.base, ErrCode::type::COM_EXCEPTION);
        LOG(ERROR) << "unRegister exception(req:" << reqbuf << ",err:" << e.what() << ").";
    }
    catch(...)
    {
        setBase(resp.base, ErrCode::type::COM_EXCEPTION);
        LOG(ERROR) << "unRegister other exception(req:" << reqbuf << ").";
    }

    LOG(INFO) << "response unRegister(seq:" << req.seq << ",resp:" << getPrint<UnRegisterResp>(resp) << ").";
}

//处理机器心跳接口
void CMasterServerHandler::heartbeat(HeartbeatResp& resp, const HeartbeatReq& req)
{
    try
    {
        SINGLETON(CResourceManager)->heartbeat(resp, req);
    }
    catch(const TTransportException& e)
    {
        setBase(resp.base, ErrCode::type::COM_EXCEPTION);
        std::string reqbuf = getPrint<HeartbeatReq>(req);
        LOG(ERROR) << "heartbeat exception(req:" << reqbuf << ",err:" << e.what() << ").";
    }
    catch(...)
    {
        setBase(resp.base, ErrCode::type::COM_EXCEPTION);
        std::string reqbuf = getPrint<HeartbeatReq>(req);
        LOG(ERROR) << "heartbeat other exception(req:" << reqbuf << ").";
    }
}

//处理上报资源接口
void CMasterServerHandler::reportResource(ReportResourceResp& resp, const ReportResourceReq& req)
{
    try
    {
        SINGLETON(CResourceManager)->reportResource(resp, req);
    }
    catch(const TTransportException& e)
    {
        setBase(resp.base, ErrCode::type::COM_EXCEPTION);
        std::string reqbuf = getPrint<ReportResourceReq>(req);
        LOG(ERROR) << "reportResource exception(req:" << reqbuf << ",err:" << e.what() << ").";
    }
    catch(...)
    {
        setBase(resp.base, ErrCode::type::COM_EXCEPTION);
        std::string reqbuf = getPrint<ReportResourceReq>(req);
        LOG(ERROR) << "reportResource other exception(req:" << reqbuf << ").";
    }
}

//上报统计信息
void CMasterServerHandler::ReportStatisticsInfo(ReportStatisticsInfoResp& resp, const ReportStatisticsInfoReq& req)
{
    try
    {
        SINGLETON(CResourceManager)->ReportStatisticsInfo(resp, req);
    }
    catch(const TTransportException& e)
    {
        setBase(resp.base, ErrCode::type::COM_EXCEPTION);
        std::string reqbuf = getPrint<ReportStatisticsInfoReq>(req);
        LOG(ERROR) << "ReportStatisticsInfo exception(req:" << reqbuf << ",err:" << e.what() << ").";
    }
    catch(...)
    {
        setBase(resp.base, ErrCode::type::COM_EXCEPTION);
        std::string reqbuf = getPrint<ReportStatisticsInfoReq>(req);
        LOG(ERROR) << "ReportStatisticsInfo other exception(req:" << reqbuf << ").";
    }
}

//获取统计信息
void CMasterServerHandler::GetStatisticsInfo(GetStatisticsInfoResp& resp, const GetStatisticsInfoReq& req)
{
    try
    {
        SINGLETON(CResourceManager)->GetStatisticsInfo(resp, req);
    }
    catch(const TTransportException& e)
    {
        setBase(resp.base, ErrCode::type::COM_EXCEPTION);
        std::string reqbuf = getPrint<GetStatisticsInfoReq>(req);
        LOG(ERROR) << "GetStatisticsInfo exception(req:" << reqbuf << ",err:" << e.what() << ").";
    }
    catch(...)
    {
        setBase(resp.base, ErrCode::type::COM_EXCEPTION);
        std::string reqbuf = getPrint<GetStatisticsInfoReq>(req);
        LOG(ERROR) << "GetStatisticsInfo other exception(req:" << reqbuf << ").";
    }
}
