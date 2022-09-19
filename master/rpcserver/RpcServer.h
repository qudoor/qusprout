#ifndef __H__RPCSERVER__H__
#define __H__RPCSERVER__H__

#include <thrift/server/TThreadPoolServer.h>
#include <thrift/server/TThreadedServer.h>

class CMasterServer
{
public:
    CMasterServer();
    ~CMasterServer();

    //启动rpc服务
    void startServer();

    //启动系统指令rpc服务
    void startSysServer();

    //停止rpc服务
    void stopServer();

private:
    bool m_isStop{false};
    ::std::shared_ptr<::apache::thrift::server::TThreadedServer> m_server{nullptr};
    ::std::shared_ptr<::apache::thrift::server::TThreadedServer> m_sysServer{nullptr};
};

#endif
