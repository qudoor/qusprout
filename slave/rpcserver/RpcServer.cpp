#include <thrift/server/TThreadedServer.h>
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TBufferTransports.h>
#include <thrift/processor/TMultiplexedProcessor.h>
#include <thrift/concurrency/ThreadFactory.h>
#include <thrift/concurrency/ThreadManager.h>
#include <transport/TTransportException.h>
#include "SlaveServerHandle.h"
#include "RpcServer.h"
#include "config/Config.h"
#include "common/qulog.h"
#include "common/Singleton.h"

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::server;
using namespace ::apache::thrift::concurrency;

CSlaveServer::CSlaveServer()
{

}

CSlaveServer::~CSlaveServer()
{

}

void CSlaveServer::startServer()
{
    try
    {
        //任务接口初始化
        ::std::shared_ptr<CSlaveServerHandler> taskhandler(new CSlaveServerHandler());
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

        LOG(INFO) << "begin start rpc server.";

        //启动服务
        m_isStop = true;
        while(m_isStop)
        {
            server->serve();
            LOG(ERROR) << "rpc server exit.";
        }

        LOG(INFO) << "start rpc server exited.";
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

void CSlaveServer::stopServer()
{
    m_isStop = false;
    LOG(INFO) << "begin rpc server is exited.";
    if (m_server != nullptr)
    {
        m_server->stop();
        m_server = nullptr;
    }
    LOG(INFO) << "rpc server is exited.";
}
