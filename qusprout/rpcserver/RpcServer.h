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
