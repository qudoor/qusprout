#ifndef __H__GATE__H__
#define __H__GATE__H__

#include <string>
#include <vector>
#include <atomic>
#include <transport/TSocket.h>
#include <transport/TBufferTransports.h>
#include <protocol/TBinaryProtocol.h>
#include <server/TSimpleServer.h>
#include <transport/TServerSocket.h>
#include <protocol/TMultiplexedProtocol.h>
#include <transport/TTransportException.h>
#include "interface/QuSproutServer.h"
#include "interface/qusprout_types.h"
#include "interface/ecode_types.h"
#include "interface/ecode_constants.h"

using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;

class CGate
{
public:
    CGate();
    ~CGate();
    bool init(const int qubitnum, const bool density = false, const std::string& ip = "127.0.0.1", const int port = 9091);

public:
    bool createPlusState();
    bool getProbAmp(const int index, double& amp);

public:
    bool hGate(const std::vector<int>& targets);
    bool mczGate(const std::vector<int>& controls, const std::vector<int>& targets);
    bool xGate(const std::vector<int>& targets);

private:
    bool createQCircuit();
    bool releaseQCircuit();

private:
    int m_qubitnum{0};
    bool m_density{false};
    std::string m_taskid{""};
    std::atomic_long m_index{0};
    std::shared_ptr<QuSproutServerClient> m_client;
    std::shared_ptr<TTransport> m_transport{nullptr};
};

#endif
