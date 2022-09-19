#ifndef __H__MASTERCLIENT__H__
#define __H__MASTERCLIENT__H__

#include <string>
#include <memory>
#include <transport/TBufferTransports.h>
#include <transport/TTransportException.h>
#include <transport/TServerSocket.h>
#include "interface/MasterServer.h"
#include "interface/resource_types.h"
#include "comm/Base.h"

using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;

class CMasterClient : public CBase
{
public:
    CMasterClient();
    virtual ~CMasterClient();

    //初始化客户端
    int init(const std::string& addr, const int port);

    //关闭客户端
    void close();

    //重新初始化客户端
    int reInit(const bool isprint = true);

public:
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

private:
    std::string m_addr{""};
    int m_port{0};
    std::shared_ptr<MasterServerClient> m_client{nullptr};
    std::shared_ptr<TTransport> m_transport{nullptr};
};

#endif 