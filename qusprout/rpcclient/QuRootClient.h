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

#ifndef __H__QUROOTCLIENT__H__
#define __H__QUROOTCLIENT__H__

#include <string>
#include <memory>
#include <transport/TBufferTransports.h>
#include <transport/TTransportException.h>
#include <transport/TServerSocket.h>
#include "interface/QuRootServer.h"
#include "interface/qurootresource_types.h"
#include "comm/Base.h"

using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;

class CQurootClient : public CBase
{
public:
    CQurootClient();
    virtual ~CQurootClient();

    //连接客户端
    int connect();
    
    //处理机器注册接口
    void resourceRegister(ResourceRegisterResp& resp, const ResourceRegisterReq& req);

    //处理机器注销接口
    void unResourceRegister(UnResourceRegisterResp& resp, const UnResourceRegisterReq& req);

    //处理机器心跳接口
    void heart(HeartResp& resp, const HeartReq& req);

private:
    //关闭客户端
    void close();

private:
    std::shared_ptr<QuRootServerClient> m_client{nullptr};
    std::shared_ptr<TTransport> m_transport{nullptr};
};

#endif 