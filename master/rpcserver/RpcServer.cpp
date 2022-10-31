#include <thrift/server/TThreadedServer.h>
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TBufferTransports.h>
#include <thrift/processor/TMultiplexedProcessor.h>
#include <thrift/concurrency/ThreadFactory.h>
#include <thrift/concurrency/ThreadManager.h>
#include <transport/TTransportException.h>
#include "QuSproutServerHandle.h"
#include "MasterServerHandle.h"
#include "RpcServer.h"
#include "config/Config.h"
#include "common/qulog.h"
#include "common/Singleton.h"

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::server;
using namespace ::apache::thrift::concurrency;

CMasterServer::CMasterServer()
{

}

CMasterServer::~CMasterServer()
{

}

void CMasterServer::startServer()
{
    try
    {
        //任务接口初始化
        ::std::shared_ptr<CQuSproutServerHandler> taskhandler(new CQuSproutServerHandler());
        ::std::shared_ptr<TProcessor> taskprocessor(new QuSproutServerProcessor(taskhandler));

        //网络、协议等初始化
        ::std::shared_ptr<TServerTransport> serverTransport(new TServerSocket(SINGLETON(CConfig)->m_listenPort, SINGLETON(CConfig)->m_sendTimeout, SINGLETON(CConfig)->m_recvTimeout));
        ::std::shared_ptr<TTransportFactory> transportFactory(new TBufferedTransportFactory());
        ::std::shared_ptr<TProtocolFactory> protocolFactory(new TBinaryProtocolFactory());

        //注册接口
        ::std::shared_ptr<TMultiplexedProcessor> multiProcessor(new TMultiplexedProcessor());
        multiProcessor->registerProcessor("QuSproutServer", taskprocessor);

        ::std::shared_ptr<TThreadedServer> server(new TThreadedServer(multiProcessor, serverTransport, transportFactory, protocolFactory));
        m_server = server;

        LOG(INFO) << "start rpc server.";

        //启动服务
        m_isStop = true;
        while(m_isStop)
        {
            server->serve();
            LOG(ERROR) << "rpc server exit.";
        }
        LOG(ERROR) << "rpc server exited.";
    }
    catch(const TTransportException& e)
    {
        LOG(ERROR) << "startServer exception(err:" << e.what() << ").";
        return;
    }
    catch(...)
    {
        LOG(ERROR) << "startServer other exception.";
        return;
    }
}

void CMasterServer::stopServer()
{
    m_isStop = false;
    if (m_server != nullptr)
    {
        m_server->stop();
        m_server = nullptr;
    }

    LOG(ERROR) << "rpc server is exited.";
}
