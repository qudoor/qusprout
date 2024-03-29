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

#include <getopt.h>
#include <iostream>
#include <thread>
#include <sstream>
#include <signal.h>
#include <thrift/server/TThreadedServer.h>
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TThreadPoolServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TBufferTransports.h>
#include <thrift/processor/TMultiplexedProcessor.h>
#include <thrift/concurrency/ThreadFactory.h>
#include <thrift/concurrency/ThreadManager.h>
#include "command/executor.h"
#include "config/Config.h"
#include "common/general.h"
#include "common/Singleton.h"
#include "common/qulog.h"
#include "common/QuESTIniter.h"
#include "WorkServerHandler.h"
#include "node/WorkNodeHandler.h"
#include "common/CmdParams.h"

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::server;
using namespace ::apache::thrift::concurrency;
using namespace std;

void ParseOpt(int argc, char **argv) 
{
    const char *optstring = "p:q:f:s:d:m:i:";

    int opt = getopt(argc, argv, optstring);
    while (opt != -1) 
    {
        //port
        if (opt == 'p') 
        {
            SINGLETON(CmdParams)->port = atoi(optarg);
        }

        //qubits
        if (opt == 'q') 
        {
             SINGLETON(CmdParams)->qubits = atoi(optarg);
        }

        //config path
        if (opt == 'f') 
        {
            SINGLETON(CmdParams)->config = optarg;
        }

        //density
        if (opt == 'd') 
        {
            SINGLETON(CmdParams)->density = atoi(optarg);
        }

        //mpi
        if (opt == 'm') 
        {
            SINGLETON(CmdParams)->execType = atoi(optarg);
        }

        opt = getopt(argc, argv, optstring);
    }
}

void startRpc() 
{
    //quroot接口初始化
    ::std::shared_ptr<CWorkServerHandler> quworkhandler(new CWorkServerHandler());
    ::std::shared_ptr<TProcessor> quworkprocessor(new QuSproutServerProcessor(quworkhandler));

    //网络、协议等初始化
    ::std::shared_ptr<TServerTransport> serverTransport(new TServerSocket(SINGLETON(CmdParams)->port));
    ::std::shared_ptr<TTransportFactory> transportFactory(new TBufferedTransportFactory());
    ::std::shared_ptr<TProtocolFactory> protocolFactory(new TBinaryProtocolFactory());

    //注册接口
    ::std::shared_ptr<TMultiplexedProcessor> multiProcessor(new TMultiplexedProcessor());
    multiProcessor->registerProcessor("QuSproutServer", quworkprocessor);

    //定义server
    ::std::shared_ptr<TThreadedServer> server(new TThreadedServer(multiProcessor, serverTransport, transportFactory, protocolFactory));

    quworkhandler->SetThreadedServer(server);

    LOG(INFO) << "server start(port:" << SINGLETON(CmdParams)->port << ")";

    //启动服务
    server->serve();
}

int main(int argc, char **argv) 
{
    SINGLETON(CQuLog);

    ParseOpt(argc, argv);

    int ret = SINGLETON(CConfig)->ParseConfig(SINGLETON(CmdParams)->config);
    if (ret != 0)
    {
        return ret;
    }

    SINGLETON(CQuLog)->Init(SINGLETON(CConfig)->m_appname, SINGLETON(CConfig)->m_logPath, SINGLETON(CConfig)->m_logLevel);
    LOG(INFO) << SINGLETON(CConfig)->GetPrintInfo();

    SINGLETON(CQuESTIniter)->Init(SINGLETON(CmdParams)->qubits, SINGLETON(CmdParams)->density);

    if(SINGLETON(CQuESTIniter)->m_env.rank == 0)
    {
        thread thd(startRpc);
        if (thd.joinable()) 
        { 
            thd.join();
        }
    }
    else
    {
        CWorkNodeHandler nodeHandler;
        nodeHandler.mpiexecute();
    }

    SINGLETON(CQuESTIniter)->finalize();
    
    return 0;
}

