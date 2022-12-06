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

#ifndef __H__GATE__H__
#define __H__GATE__H__

#include <string>
#include <vector>
#include <atomic>
#include <fstream>
#include <iostream>
#include <sstream>
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

//打印rpc的结构
template <typename T>
inline std::string getPrint(const T& req)
{
    std::stringstream os("");
    req.printTo(os);
    return os.str();
}

class CGate
{
public:
    CGate();
    ~CGate();
    bool init(const int qubitnum, const bool density = false, const std::string& ip = "127.0.0.1", const int port = 9091);

public:
    bool createQCircuit();
    bool createPlusState();
    bool getProbAmp(const int index, double& amp);
    bool run(const int shots, MeasureResult& result);
    bool releaseQCircuit();

public:
    bool hGate(const std::vector<int>& targets);
    bool mczGate(const std::vector<int>& controls, const std::vector<int>& targets);
    bool xGate(const std::vector<int>& targets);
    bool cnotGate(const std::vector<int>& controls, const std::vector<int>& targets);
    bool measureGate(const std::vector<int>& targets);

    bool endAdd();

private:
    int m_qubitnum{0};
    bool m_density{false};
    std::string m_taskid{""};
    std::atomic_long m_index{0};
    std::shared_ptr<QuSproutServerClient> m_client{nullptr};
    std::shared_ptr<TTransport> m_transport{nullptr};
    bool m_isrelease{false};
};

#endif
