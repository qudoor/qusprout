#ifndef __H__RPCSERVER__H__
#define __H__RPCSERVER__H__

#include <thrift/server/TThreadPoolServer.h>
#include <thrift/server/TThreadedServer.h>

class CQuSproutServer
{
public:
    CQuSproutServer();
    ~CQuSproutServer();

    //启动rpc服务
    void startServer();

    //停止rpc服务
    void stopServer();

private:
    bool m_isStop{false};
    ::std::shared_ptr<::apache::thrift::server::TThreadedServer> m_server{nullptr};
};

#endif
