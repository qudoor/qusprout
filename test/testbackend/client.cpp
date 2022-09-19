#include <transport/TSocket.h>
#include <transport/TBufferTransports.h>
#include <protocol/TBinaryProtocol.h>
#include <server/TSimpleServer.h>
#include <transport/TServerSocket.h>
#include <protocol/TMultiplexedProtocol.h>
#include <transport/TTransportException.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include "QuSproutServer.h"
#include "qusprout_types.h"
#include "ecode_types.h"
#include "ecode_constants.h"
#include "MasterServer.h"
#include "resource_types.h"
#include "statistics_types.h"

using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;

void testSimMultiCmd()
{
    std::shared_ptr<TSocket> socket = std::make_shared<TSocket>("192.168.158.129", 9091); 
    std::shared_ptr<TTransport> transport = std::make_shared<TBufferedTransport>(socket);
    std::shared_ptr<TProtocol> protocol = std::make_shared<TBinaryProtocol>(transport);
    std::shared_ptr<TMultiplexedProtocol> quest = std::make_shared<TMultiplexedProtocol>(protocol, "QuSproutServer");
    transport->open();

    std::shared_ptr<QuSproutServerClient> client = std::make_shared<QuSproutServerClient>(quest);

    //初始化
    InitQubitsReq initreq;
    initreq.__set_id("1");
    initreq.__set_qubits(10);
    initreq.__set_state (InitState::Zero);
    initreq.__set_density(false);
    initreq.__set_value(0);
    InitQubitsResp initresp;
    client->initQubits(initresp, initreq);
    std::ostream& infoos = std::cout;
    std::cout << "begin init---------------" << std::endl;
    initresp.printTo(infoos);
    std::cout << std::endl << "end init---------------" << std::endl;

    const int threadnum = 15;
    std::thread* threadlist[threadnum];
    for (auto i = 0; i < threadnum; ++i)
    {
        threadlist[i] = new std::thread([&client, initreq](const int index) {
            //发送指令
            SendCircuitCmdReq cmdreq;
            cmdreq.__set_id("1");
            Circuit circuit;
            for (auto i = 0; i < initreq.qubits; ++i)
            {
                Cmd cmd;
                cmd.__set_gate("H");
                cmd.targets.push_back(i);
                std::ostringstream os1("");
                os1 << "h q[" << i << "]";
                cmd.desc = os1.str();

                circuit.cmds.push_back(cmd);
            }
            cmdreq.__set_circuit(circuit);
            cmdreq.__set_final(false);
            SendCircuitCmdResp cmdresp;
            int count = 1000;
            while(count > 0)
            {
                client->sendCircuitCmd(cmdresp, cmdreq);
                count--;
            }
            std::ostream& infoos1 = std::cout;
            std::cout << "begin cmd---------------" << index << std::endl;
            cmdresp.printTo(infoos1);
            std::cout << std::endl << "end cmd---------------" << index << std::endl;
        }, i);
    }

    for (auto i = 0; i < threadnum; ++i)
    {
        threadlist[i]->join();
        delete threadlist[i];
    }

    //std::this_thread::sleep_for(std::chrono::milliseconds(1000*1));
    
    transport->close();
}

void testSimThreadCmd()
{
    const int threadnum = 1;
    std::thread* threadlist[threadnum];
    for (auto i = 0; i < threadnum; ++i)
    {
        threadlist[i] = new std::thread([](const int index) {
            while(1)
            {
                std::ostringstream os("");
                os << time(NULL) << index;
                auto id = os.str();

                try
                {
                    std::cout << "1---------------" << std::endl;
                    std::shared_ptr<TSocket> socket = std::make_shared<TSocket>("192.168.158.134", 9091); 
                    socket->setConnTimeout(60000);
                    socket->setRecvTimeout(60000);
                    socket->setSendTimeout(60000);
                    std::cout << "2---------------" << std::endl;
                    std::shared_ptr<TTransport> transport = std::make_shared<TBufferedTransport>(socket);
                    std::cout << "3---------------" << std::endl;
                    std::shared_ptr<TProtocol> protocol = std::make_shared<TBinaryProtocol>(transport);
                    std::cout << "4---------------" << std::endl;
                    std::shared_ptr<TMultiplexedProtocol> quest = std::make_shared<TMultiplexedProtocol>(protocol, "QuSproutServer");
                    std::cout << "5---------------" << std::endl;
                    transport->open();
                    std::cout << "6---------------" << std::endl;

                    std::shared_ptr<QuSproutServerClient> client = std::make_shared<QuSproutServerClient>(quest);
                    std::cout << "7---------------" << std::endl;

                    //初始化
                    InitQubitsReq initreq;
                    initreq.__set_id(id);
                    initreq.__set_qubits(15);
                    initreq.__set_state (InitState::Classical);
                    initreq.__set_density(false);
                    initreq.__set_value(0);
                    initreq.__set_exec_type(ExecCmdType::ExecTypeGpuSingle);
                    InitQubitsResp initresp;
                    client->initQubits(initresp, initreq);
                    std::cout << "begin init---------------" << std::endl;
                    std::ostream& infoos = std::cout;
                    initresp.printTo(infoos);
                    std::cout << std::endl << "end init---------------" << std::endl;

                    sleep(2);

                    //发送指令
                    SendCircuitCmdReq cmdreq;
                    cmdreq.__set_id(id);
                    Circuit circuit;
                    for (auto i = 0; i < initreq.qubits; ++i)
                    {
                        Cmd cmd;
                        cmd.__set_gate("H");
                        cmd.targets.push_back(i);
                        std::ostringstream os1("");
                        os1 << "h q[" << i << "]";
                        cmd.desc = os1.str();

                        circuit.cmds.push_back(cmd);
                    }
                    cmdreq.__set_circuit(circuit);
                    cmdreq.__set_final(false);
                    SendCircuitCmdResp cmdresp;
                    client->sendCircuitCmd(cmdresp, cmdreq);
                    std::ostream& infoos1 = std::cout;
                    std::cout << "begin cmd---------------" << index << std::endl;
                    cmdresp.printTo(infoos1);
                    std::cout << std::endl << "end cmd---------------" << index << std::endl;

                    //获取所有的计算状态
                    GetAllStateReq statereq;
                    statereq.__set_id(id);
                    GetAllStateResp stateresp;
                    client->getAllState(stateresp, statereq);
                    std::ostream& infoos3 = std::cout;
                    std::cout << "begin getAllState---------------" << index << std::endl;
                    stateresp.printTo(infoos3);
                    std::cout << std::endl << "end getAllState---------------" << index << std::endl;

                    //释放资源
                    SendCircuitCmdReq cmdreq1;
                    cmdreq1.__set_id(id);
                    cmdreq1.__set_final(true);
                    SendCircuitCmdResp cmdresp1;
                    client->sendCircuitCmd(cmdresp1, cmdreq1);
                    std::ostream& infoos4 = std::cout;
                    std::cout << "begin final cmd---------------" << index << std::endl;
                    cmdresp1.printTo(infoos4);
                    std::cout << std::endl << "end final cmd---------------" << index << std::endl;

                    //获取所有的计算状态
                    RunCircuitReq runreq;
                    runreq.__set_id(id);
                    runreq.__set_shots(1);
                    RunCircuitResp runresp;
                    client->run(runresp, runreq);
                    std::ostream& infoos2 = std::cout;
                    std::cout << "begin run---------------" << index << std::endl;
                    stateresp.printTo(infoos2);
                    std::cout << std::endl << "end run---------------" << index << std::endl;

                    /*
                    //取消任务
                    CancelCmdReq cancelreq;
                    cancelreq.__set_id(id);
                    CancelCmdResp cancelresp;
                    client->cancelCmd(cancelresp, cancelreq);
                    std::ostream& infoos5 = std::cout;
                    std::cout << "begin cancelCmd---------------" << index << std::endl;
                    cmdresp1.printTo(infoos5);
                    std::cout << std::endl << "end cancelCmd---------------" << index << std::endl;
                    */

                    transport->close();
                    sleep(60);
                }
                catch(const TTransportException& e)
                {
                    std::cout << "init exception(err:" << e.what() << ",getType:" << e.getType() << ").";
                }
                catch(...)
                {
                    std::cout << "init other exception.";
                }
            }
        }, i);
    }

    for (auto i = 0; i < threadnum; ++i)
    {
        threadlist[i]->join();
        delete threadlist[i];
    }

    //std::this_thread::sleep_for(std::chrono::milliseconds(1000*1));
}

void testTaskState()
{
    std::shared_ptr<TSocket> socket = std::make_shared<TSocket>("192.168.158.129", 9091); 
    std::shared_ptr<TTransport> transport = std::make_shared<TBufferedTransport>(socket);
    std::shared_ptr<TProtocol> protocol = std::make_shared<TBinaryProtocol>(transport);
    std::shared_ptr<TMultiplexedProtocol> quest = std::make_shared<TMultiplexedProtocol>(protocol, "QuSproutServer");
    transport->open();

    std::shared_ptr<QuSproutServerClient> client = std::make_shared<QuSproutServerClient>(quest);

    //初始化
    GetAllStateReq statereq;
    statereq.__set_id("2");
    GetAllStateResp stateresp;
    client->getAllState(stateresp, statereq);
    std::cout << "begin getAllState---------------" << std::endl;
    std::ostream& os = std::cout;
    stateresp.printTo(os);
    std::cout << std::endl << "end getAllState---------------" << std::endl;

    transport->close();
}

void testTaskFree()
{
    std::shared_ptr<TSocket> socket = std::make_shared<TSocket>("192.168.158.129", 9091); 
    std::shared_ptr<TTransport> transport = std::make_shared<TBufferedTransport>(socket);
    std::shared_ptr<TProtocol> protocol = std::make_shared<TBinaryProtocol>(transport);
    std::shared_ptr<TMultiplexedProtocol> quest = std::make_shared<TMultiplexedProtocol>(protocol, "QuSproutServer");
    transport->open();

    std::shared_ptr<QuSproutServerClient> client = std::make_shared<QuSproutServerClient>(quest);

    //初始化
    InitQubitsReq initreq;
    initreq.__set_id("1");
    initreq.__set_qubits(15);
    initreq.__set_state (InitState::Zero);
    initreq.__set_density(false);
    initreq.__set_value(0);
    InitQubitsResp initresp;
    client->initQubits(initresp, initreq);
    std::cout << "begin init---------------" << std::endl;
    std::ostream& infoos = std::cout;
    initresp.printTo(infoos);
    std::cout << std::endl << "end init---------------" << std::endl;

    //发送指令
    SendCircuitCmdReq cmdreq;
    cmdreq.__set_id("1");
    Circuit circuit;
    cmdreq.__set_circuit(circuit);
    cmdreq.__set_final(true);
    SendCircuitCmdResp cmdresp;
    client->sendCircuitCmd(cmdresp, cmdreq);
    client->sendCircuitCmd(cmdresp, cmdreq);
    client->sendCircuitCmd(cmdresp, cmdreq);

    std::ostream& infoos1 = std::cout;
    std::cout << "begin cmd---------------" << std::endl;
    cmdresp.printTo(infoos1);
    std::cout << std::endl << "end cmd---------------" << std::endl;

    transport->close();
}

void testGetStatisticsInfo()
{
    std::shared_ptr<TSocket> socket = std::make_shared<TSocket>("192.168.158.134", 9092); 
    std::shared_ptr<TTransport> transport = std::make_shared<TBufferedTransport>(socket);
    std::shared_ptr<TProtocol> protocol = std::make_shared<TBinaryProtocol>(transport);
    std::shared_ptr<TMultiplexedProtocol> quest = std::make_shared<TMultiplexedProtocol>(protocol, "MasterServer");
    transport->open();

    std::shared_ptr<MasterServerClient> client = std::make_shared<MasterServerClient>(quest);

    //获取统计信息
    GetStatisticsInfoReq getreq;
    getreq.__set_seq("1");
    GetStatisticsInfoResp getresp;
    client->GetStatisticsInfo(getresp, getreq);
    std::cout << "begin GetStatisticsInfo---------------" << std::endl;
    std::ostream& infoos = std::cout;
    getresp.printTo(infoos);
    std::cout << std::endl << "end GetStatisticsInfo---------------" << std::endl;

    transport->close();
}

int main(int argc, char **argv) {
    //testSimMultiCmd();
    //testSimThreadCmd();
    //testTaskState();
    //testTaskFree();
    testGetStatisticsInfo();
    return 0;
}
