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
#include <stdlib.h>
#include <math.h>
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
    std::shared_ptr<TSocket> socket = std::make_shared<TSocket>("192.168.158.146", 9091); 
    std::shared_ptr<TTransport> transport = std::make_shared<TBufferedTransport>(socket);
    std::shared_ptr<TProtocol> protocol = std::make_shared<TBinaryProtocol>(transport);
    std::shared_ptr<TMultiplexedProtocol> quest = std::make_shared<TMultiplexedProtocol>(protocol, "QuSproutServer");
    transport->open();

    std::shared_ptr<QuSproutServerClient> client = std::make_shared<QuSproutServerClient>(quest);

    //初始化
    InitQubitsReq initreq;
    initreq.__set_id("1");
    initreq.__set_qubits(10);
    initreq.__set_density(false);
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
            for (auto i = 1; i <= initreq.qubits; ++i)
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
                    std::shared_ptr<TSocket> socket = std::make_shared<TSocket>("192.168.158.146", 9091); 
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
                    initreq.__set_qubits(2);
                    initreq.__set_density(false);
                    initreq.__set_exec_type(ExecCmdType::ExecTypeCpuSingle);
                    InitQubitsResp initresp;
                    client->initQubits(initresp, initreq);
                    std::cout << "begin init---------------" << std::endl;
                    std::ostream& infoos = std::cout;
                    initresp.printTo(infoos);
                    std::cout << std::endl << "end init---------------" << std::endl;

                    sleep(2);

                    for (int j = 0; j < 2; j++) {
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
                    }

                   //获取所有的计算状态
                    RunCircuitReq runreq;
                    runreq.__set_id(id);
                    runreq.__set_shots(1);
                    RunCircuitResp runresp;
                    client->run(runresp, runreq);
                    std::ostream& infoos2 = std::cout;
                    std::cout << "begin run---------------" << index << std::endl;
                    runresp.printTo(infoos2);
                    std::cout << std::endl << "end run---------------" << index << std::endl;
              
                    /*
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
                    */

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
    std::shared_ptr<TSocket> socket = std::make_shared<TSocket>("192.168.158.146", 9091); 
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
    std::shared_ptr<TSocket> socket = std::make_shared<TSocket>("192.168.158.146", 9091); 
    std::shared_ptr<TTransport> transport = std::make_shared<TBufferedTransport>(socket);
    std::shared_ptr<TProtocol> protocol = std::make_shared<TBinaryProtocol>(transport);
    std::shared_ptr<TMultiplexedProtocol> quest = std::make_shared<TMultiplexedProtocol>(protocol, "QuSproutServer");
    transport->open();

    std::shared_ptr<QuSproutServerClient> client = std::make_shared<QuSproutServerClient>(quest);

    //初始化
    InitQubitsReq initreq;
    initreq.__set_id("1");
    initreq.__set_qubits(15);
    initreq.__set_density(false);
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
    std::shared_ptr<TSocket> socket = std::make_shared<TSocket>("192.168.158.146", 9092); 
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

void testpauil()
{
    std::ostringstream os("");
    os << time(NULL);
    auto id = os.str();

    int qubitnum = 5;
    try
    {
        std::shared_ptr<TSocket> socket = std::make_shared<TSocket>("192.168.158.146", 9091); 
        socket->setConnTimeout(60000);
        socket->setRecvTimeout(60000);
        socket->setSendTimeout(60000);
        std::shared_ptr<TTransport> transport = std::make_shared<TBufferedTransport>(socket);
        std::shared_ptr<TProtocol> protocol = std::make_shared<TBinaryProtocol>(transport);
        std::shared_ptr<TMultiplexedProtocol> quest = std::make_shared<TMultiplexedProtocol>(protocol, "QuSproutServer");
        transport->open();

        std::shared_ptr<QuSproutServerClient> client = std::make_shared<QuSproutServerClient>(quest);

        //初始化
        InitQubitsReq initreq;
        initreq.__set_id(id);
        initreq.__set_qubits(qubitnum);
        initreq.__set_density(false);
        initreq.__set_exec_type(ExecCmdType::ExecTypeCpuMpi);
        InitQubitsResp initresp;
        client->initQubits(initresp, initreq);
        std::cout << "begin init---------------" << std::endl;

        //发送指令
        SendCircuitCmdReq cmdreq;
        cmdreq.__set_id(id);
        Circuit circuit;
        for (auto i = 0; i < qubitnum; ++i)
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

        //获取所有的计算状态
        GetAllStateReq statereq;
        statereq.__set_id(id);
        GetAllStateResp stateresp;
        client->getAllState(stateresp, statereq);

        time_t now =time(NULL);

        //获取泡利算子乘积的期望值
        GetExpecPauliProdReq pauliprodreq;
        pauliprodreq.__set_id(id);
        std::vector<PauliProdInfo> pauliprod;
        for (auto i = 0; i < qubitnum; ++i)
        {
            PauliProdInfo temp;
            temp.__set_target(i);
            temp.__set_oper_type((PauliOperType::type)(rand() % 4));
            pauliprod.push_back(temp);
        }
        pauliprodreq.__set_pauli_prod(pauliprod);
        GetExpecPauliProdResp pauliprodresp;
        client->getExpecPauliProd(pauliprodresp, pauliprodreq);
        std::ostream& infoos3 = std::cout;
        pauliprodreq.printTo(infoos3);
        pauliprodresp.printTo(infoos3);
        std::cout << std::endl << "end getExpecPauliProd---------------" << std::endl;

        int termsnum = 1;
        std::vector<PauliOperType::type> oper_type_list;
        std::vector<double> term_coeff_list;
        GetExpecPauliSumReq paulisumreq;
        paulisumreq.__set_id(id);
        for (auto i = 0; i < termsnum*qubitnum; ++i)
        {
            srand(now + i);
            oper_type_list.push_back((PauliOperType::type)(rand() % 4));

            double coeff = rand() % 1000;
            coeff += coeff/100.0;
            if (rand() % 2 == 0)
            {
                coeff = -coeff;
            }
            term_coeff_list.push_back(coeff);
        }
        paulisumreq.__set_oper_type_list(oper_type_list);
        paulisumreq.__set_term_coeff_list(term_coeff_list);
        GetExpecPauliSumResp paulisumresp;
        client->getExpecPauliSum(paulisumresp, paulisumreq);
        std::ostream& infoos4 = std::cout;
        paulisumreq.printTo(infoos4);
        paulisumresp.printTo(infoos4);
        std::cout << std::endl << "end getExpecPauliSum---------------" << std::endl;

        //运行
        RunCircuitReq runreq;
        runreq.__set_id(id);
        runreq.__set_shots(1);
        RunCircuitResp runresp;
        client->run(runresp, runreq);

        transport->close();
        sleep(1);
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

void testpauil1()
{
    std::ostringstream os("");
    os << time(NULL);
    auto id = os.str();

    int qubitnum = 5;
    try
    {
        std::shared_ptr<TSocket> socket = std::make_shared<TSocket>("192.168.158.146", 9091); 
        socket->setConnTimeout(60000);
        socket->setRecvTimeout(60000);
        socket->setSendTimeout(60000);
        std::shared_ptr<TTransport> transport = std::make_shared<TBufferedTransport>(socket);
        std::shared_ptr<TProtocol> protocol = std::make_shared<TBinaryProtocol>(transport);
        std::shared_ptr<TMultiplexedProtocol> quest = std::make_shared<TMultiplexedProtocol>(protocol, "QuSproutServer");
        transport->open();

        std::shared_ptr<QuSproutServerClient> client = std::make_shared<QuSproutServerClient>(quest);

        //初始化
        InitQubitsReq initreq;
        initreq.__set_id(id);
        initreq.__set_qubits(qubitnum);
        initreq.__set_density(false);
        initreq.__set_exec_type(ExecCmdType::ExecTypeCpuMpi);
        InitQubitsResp initresp;
        client->initQubits(initresp, initreq);
        std::ostream& infoos = std::cout;
        initreq.printTo(infoos);
        std::cout << "begin init---------------" << std::endl;

        //发送指令
        SendCircuitCmdReq cmdreq;
        cmdreq.__set_id(id);
        Circuit circuit;
        for (auto i = 0; i < qubitnum; ++i)
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

        //获取所有的计算状态
        GetAllStateReq statereq;
        statereq.__set_id(id);
        GetAllStateResp stateresp;
        client->getAllState(stateresp, statereq);

        time_t now =time(NULL);

        //获取泡利算子乘积的期望值
        GetExpecPauliProdReq pauliprodreq;
        pauliprodreq.__set_id(id);
        std::vector<PauliProdInfo> pauliprod;
        for (auto i = 0; i < qubitnum; ++i)
        {
            PauliProdInfo temp;
            temp.__set_target(i);
            switch(i) 
            {
            case 0:
                temp.__set_oper_type(PauliOperType::type::POT_PAULI_I);
                break;
            case 1:
                temp.__set_oper_type(PauliOperType::type::POT_PAULI_I);
                break;
            case 2:
                temp.__set_oper_type(PauliOperType::type::POT_PAULI_X);
                break;
            case 3:
                temp.__set_oper_type(PauliOperType::type::POT_PAULI_I);
                break;
            case 4:
                temp.__set_oper_type(PauliOperType::type::POT_PAULI_X);
                break;
            }
            pauliprod.push_back(temp);
        }
        pauliprodreq.__set_pauli_prod(pauliprod);
        GetExpecPauliProdResp pauliprodresp;
        client->getExpecPauliProd(pauliprodresp, pauliprodreq);
        std::ostream& infoos3 = std::cout;
        pauliprodreq.printTo(infoos3);
        pauliprodresp.printTo(infoos3);
        std::cout << std::endl << "end getExpecPauliProd---------------" << std::endl;

        int termsnum = 1;
        //获取泡利算子乘积之和的期望值
        GetExpecPauliSumReq paulisumreq;
        paulisumreq.__set_id(id);
        std::vector<PauliOperType::type> oper_type_list;
        std::vector<double> term_coeff_list;
        for (auto i = 0; i < termsnum*qubitnum; ++i)
        {
            srand(now + i);
            switch(i) 
            {
            case 0:
                oper_type_list.push_back(PauliOperType::type::POT_PAULI_I);
                term_coeff_list.push_back(467.63);
                break;
            case 1:
                oper_type_list.push_back(PauliOperType::type::POT_PAULI_X);
                term_coeff_list.push_back(652.46000000000004);
                break;
            case 2:
                oper_type_list.push_back(PauliOperType::type::POT_PAULI_X);
                term_coeff_list.push_back(249.47);
                break;
            case 3:
                oper_type_list.push_back(PauliOperType::type::POT_PAULI_X);
                term_coeff_list.push_back(-324.20999999999998);
                break;
            case 4:
                oper_type_list.push_back(PauliOperType::type::POT_PAULI_I);
                term_coeff_list.push_back(-343.39999999999998);
                break;
            }
        }
        paulisumreq.__set_oper_type_list(oper_type_list);
        paulisumreq.__set_term_coeff_list(term_coeff_list);
        GetExpecPauliSumResp paulisumresp;
        client->getExpecPauliSum(paulisumresp, paulisumreq);
        std::ostream& infoos4 = std::cout;
        paulisumreq.printTo(infoos4);
        paulisumresp.printTo(infoos4);
        std::cout << std::endl << "end getExpecPauliSum---------------" << std::endl;

        //运行
        RunCircuitReq runreq;
        runreq.__set_id(id);
        runreq.__set_shots(1);
        RunCircuitResp runresp;
        client->run(runresp, runreq);

        transport->close();
        sleep(1);
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

void testget()
{
    std::ostringstream os("");
    os << time(NULL);
    auto id = os.str();

    int qubitnum = 3;
    try
    {
        std::shared_ptr<TSocket> socket = std::make_shared<TSocket>("192.168.158.146", 9091); 
        socket->setConnTimeout(60000);
        socket->setRecvTimeout(60000);
        socket->setSendTimeout(60000);
        std::shared_ptr<TTransport> transport = std::make_shared<TBufferedTransport>(socket);
        std::shared_ptr<TProtocol> protocol = std::make_shared<TBinaryProtocol>(transport);
        std::shared_ptr<TMultiplexedProtocol> quest = std::make_shared<TMultiplexedProtocol>(protocol, "QuSproutServer");
        transport->open();

        std::shared_ptr<QuSproutServerClient> client = std::make_shared<QuSproutServerClient>(quest);

        //初始化
        InitQubitsReq initreq;
        initreq.__set_id(id);
        initreq.__set_qubits(qubitnum);
        initreq.__set_density(false);
        initreq.__set_exec_type(ExecCmdType::ExecTypeCpuSingle);
        InitQubitsResp initresp;
        client->initQubits(initresp, initreq);
        std::ostream& osstream = std::cout;
        initreq.printTo(osstream);
        std::cout << "end init---------------" << std::endl;

        //发送指令
        SendCircuitCmdReq cmdreq;
        cmdreq.__set_id(id);
        Circuit circuit;
        for (auto i = 0; i < qubitnum; ++i)
        {
            Cmd cmd;
            cmd.__set_gate("H");
            cmd.targets.push_back(i);
            std::ostringstream os("");
            os << "h q[" << i << "]";
            cmd.desc = os.str();
            circuit.cmds.push_back(cmd);

            Cmd cmd1;
            cmd1.__set_gate("Z");
            cmd1.targets.push_back(i);
            os.str("");
            os << "z q[" << i << "]";
            cmd1.desc = os.str();
            cmd1.inverse = false;
            circuit.cmds.push_back(cmd1);
			
			Cmd cmd2;
            cmd2.__set_gate("Measure");
            cmd2.targets.push_back(i);
            os.str("");
            os << "Measure q[" << i << "]";
            cmd2.desc = os.str();
            circuit.cmds.push_back(cmd2);
        }
        cmdreq.__set_circuit(circuit);
        cmdreq.__set_final(true);
        SendCircuitCmdResp cmdresp;
        client->sendCircuitCmd(cmdresp, cmdreq);
        std::cout << "end sendCircuitCmd---------------" << std::endl;
/*
        //添加量子门操作
        AddSubCircuitReq addsubreq;
        addsubreq.__set_id(id);
        SubCircuit subcmd;
        subcmd.__set_name("H");
        Circuit circuit1;
        for (auto i = 0; i < qubitnum; ++i)
        {
            Cmd cmd;
            cmd.__set_gate("H");
            cmd.targets.push_back(i);
            std::ostringstream os("");
            os << "h q[" << i << "]";
            cmd.desc = os.str();
            circuit1.cmds.push_back(cmd);
        }
        subcmd.__set_circuit(circuit1);
        addsubreq.__set_sub_circuit(subcmd);
        AddSubCircuitResp addsubresp;
        client->addSubCircuit(addsubresp, addsubreq);
        addsubresp.printTo(osstream);
        std::cout << "end addSubCircuit---------------" << std::endl;
*/
        //获取所有的计算状态
        GetAllStateReq statereq;
        statereq.__set_id(id);
        GetAllStateResp stateresp;
        client->getAllState(stateresp, statereq);
        stateresp.printTo(osstream);
        std::cout << "end getAllState---------------" << std::endl;

        //获取振幅
        for (auto i = 0; i < pow(2,qubitnum); ++i)
        {
            GetProbAmpReq ampreq;
            ampreq.__set_id(id);
            ampreq.__set_index(i);
            GetProbAmpResp ampresp;
            client->getProbAmp(ampresp, ampreq);
            ampresp.printTo(osstream);
            std::cout << "end getProbAmp---------------" << std::endl;
        }

        //获取当前qubit的概率
        for (auto i = 0; i < qubitnum; ++i)
        {
            GetProbOfOutcomeReq alloutcomreq;
            alloutcomreq.__set_id(id);
            alloutcomreq.__set_qubit(i);
            alloutcomreq.__set_outcom(0);
            GetProbOfOutcomeResp alloutcomresp;
            client->getProbOfOutcome(alloutcomresp, alloutcomreq);
            alloutcomresp.printTo(osstream);
            std::cout << "end getProbOfOutcome---------------" << std::endl;
        }

        GetProbOfAllOutcomReq alloutcomreqex;
        alloutcomreqex.__set_id(id);
        std::vector<int32_t> outcomreex;
        outcomreex.push_back(0);
        outcomreex.push_back(1);
        alloutcomreqex.__set_targets(outcomreex);
        GetProbOfAllOutcomResp alloutcomrespex;
        client->getProbOfAllOutcome(alloutcomrespex, alloutcomreqex);
        alloutcomrespex.printTo(osstream);
        std::cout << "end getProbOfAllOutcome---------------" << std::endl;

        //获取测量结果
        MeasureQubitsReq measurereq;
        measurereq.__set_id(id);
        std::vector<int32_t> measurequbits;
        for (auto i = 0; i < qubitnum; ++i)
        {
            measurequbits.push_back(i);
        }
        measurereq.__set_qubits(measurequbits);
        MeasureQubitsResp measureresp;
        client->measureQubits(measureresp, measurereq);
        measureresp.printTo(osstream);
        std::cout << "end measureQubits---------------" << std::endl;
/*
        //注册一些自定义量子门，单次任务有效
        AddCustomGateByMatrixReq addmatrixreq;
        addmatrixreq.__set_id(id);
        GateMatrix matrixgate;
        matrixgate.__set_name("test");
        matrixgate.__set_qubits(2);
        std::vector<double> matrixlist;
        //0,0,   0,-1,   0,1,   0,0
        matrixlist.push_back(1);
        matrixlist.push_back(0);
        matrixlist.push_back(0);
        matrixlist.push_back(-1);
        matrixlist.push_back(0);
        matrixlist.push_back(1);
        matrixlist.push_back(0);
        matrixlist.push_back(1);
        matrixlist.push_back(0);
        matrixlist.push_back(0);
        matrixlist.push_back(0);
        matrixlist.push_back(-1);
        matrixlist.push_back(0);
        matrixlist.push_back(1);
        matrixlist.push_back(0);
        matrixlist.push_back(1);
        matrixgate.__set_matrix(matrixlist);
        addmatrixreq.__set_gate(matrixgate);
        AddCustomGateByMatrixResp addmatrixresp;
        client->addCustomGateByMatrix(addmatrixresp, addmatrixreq);
        addmatrixresp.printTo(osstream);
        std::cout << "end addCustomGateByMatrix---------------" << std::endl;

        //获取当前量子状态向量
        GetStateOfAllQubitsReq allstatereq;
        allstatereq.__set_id(id);
        GetStateOfAllQubitsResp allstateresp;
        client->getStateOfAllQubits(allstateresp, allstatereq);
        allstateresp.printTo(osstream);
        std::cout << "end getStateOfAllQubits---------------" << std::endl;

        //获取当前所有可能状态组合的概率
        GetProbabilitiesReq probreq;
        probreq.__set_id(id);
        GetProbabilitiesResp probresp;
        client->getProbabilities(probresp, probreq);
        probresp.printTo(osstream);
        std::cout << "end getProbabilities---------------" << std::endl;

        //重置指定的qubits
        ResetQubitsReq resetreq;
        resetreq.__set_id(id);
        std::vector<int32_t> resetqubits;
        resetqubits.push_back(0);
        resetreq.__set_qubits(resetqubits);
        ResetQubitsResp resetresp;
        client->resetQubits(resetresp, resetreq);
        resetresp.printTo(osstream);
        std::cout << "end resetQubits---------------" << std::endl;

        //追加量子比特到当前的量子电路
        AppendQubitsReq appendreq;
        appendreq.__set_id(id);
        appendreq.__set_qubits(1);
        AppendQubitsResp appendresp;
        client->appendQubits(appendresp, appendreq);
        appendresp.printTo(osstream);
        std::cout << "end appendQubits---------------" << std::endl;

        //添加量子门操作
        AddSubCircuitReq addsubreqex;
        addsubreqex.__set_id(id);
        SubCircuit subcmdex;
        subcmdex.__set_name("test");
        Circuit circuitex;
        Cmd cmdex;
        cmdex.__set_gate("test");
        os.str("");
        os << "test q[0],q[1],q[3]";
        cmdex.desc = os.str();
        for (auto i = 0; i < qubitnum + 1; ++i)
        {
            cmdex.targets.push_back(i);
        }
        circuitex.cmds.push_back(cmdex);
        subcmdex.__set_circuit(circuitex);
        addsubreqex.__set_sub_circuit(subcmdex);
        AddSubCircuitResp addsubrespex;
        client->addSubCircuit(addsubrespex, addsubreqex);
        addsubrespex.printTo(osstream);
        std::cout << "end addSubCircuitex---------------" << std::endl;

        //获取当前所有可能状态组合的概率
        GetProbabilitiesReq probreq1;
        probreq1.__set_id(id);
        GetProbabilitiesResp probresp1;
        client->getProbabilities(probresp1, probreq1);
        probresp1.printTo(osstream);
        std::cout << "end getProbabilities1---------------" << std::endl;
*/
        //运行
        RunCircuitReq runreq;
        runreq.__set_id(id);
        runreq.__set_shots(5);
        RunCircuitResp runresp;
        client->run(runresp, runreq);
        runresp.printTo(osstream);
        std::cout << "end run---------------" << std::endl;

        transport->close();
        sleep(1);
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

int main(int argc, char **argv) {
    //testSimMultiCmd();
    //testSimThreadCmd();
    //testTaskState();
    //testTaskFree();
    //testGetStatisticsInfo();
    //testpauil();
    //testpauil1();
    testget();
    
    return 0;
}
