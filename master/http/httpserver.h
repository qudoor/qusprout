#ifndef __H__HTTPSERVER__H__
#define __H__HTTPSERVER__H__

#include <hv/HttpService.h>
#include <hv/HttpServer.h>

class CHttpServer
{
public:
    CHttpServer();
    ~CHttpServer();

    int init();
    void start();
    void stop();

private:
    hv::HttpServer* m_server{nullptr};
    hv::HttpService* m_service{nullptr};
};

#endif 
