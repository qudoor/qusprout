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

#include <hv/hsysinfo.h>
#include "common/qulog.h"
#include "config/Config.h"
#include "common/Singleton.h"
#include "httpserver.h"
#include "router.h"

CHttpServer::CHttpServer()
{

}

CHttpServer::~CHttpServer()
{
}

int CHttpServer::init()
{
    if (nullptr == m_service)
    {
        m_service = new hv::HttpService();
        if (nullptr == m_service)
        {
            LOG(ERROR) << "new HttpService failed.";
            return -1;
        }
    }

    if (nullptr == m_server)
    {
        m_server = new hv::HttpServer();
        if (nullptr == m_server)
        {
            LOG(ERROR) << "new HttpServer failed.";
            return -2;
        }
    }

    return 0;
}

void CHttpServer::start()
{
    CRouter::Register(m_service);

    std::string listenip = "0.0.0.0";
    if (SINGLETON(CConfig)->m_listenAddr != "")
    {
        listenip = SINGLETON(CConfig)->m_listenAddr;
    }
    m_server->setHost(listenip.c_str());

    m_server->setPort(SINGLETON(CConfig)->m_listenHttpPort);

    int threadnum = SINGLETON(CConfig)->m_httpThreadNum;
    if (threadnum <= 0)
    {
        threadnum = get_ncpu();
    }
    m_server->setThreadNum(threadnum);

    m_server->registerHttpService(m_service);
    m_server->run();
}

void CHttpServer::stop()
{
    if (nullptr != m_server)
    {
        m_server->stop();
        delete m_server;
        m_server = nullptr;
    }

    if (nullptr != m_service)
    {
        delete m_service;
        m_service = nullptr;
    }

    LOG(INFO) << "http server stoped.";
}