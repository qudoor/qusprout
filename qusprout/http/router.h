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

#ifndef __H__ROUTER__H__
#define __H__ROUTER__H__

#include <hv/HttpService.h>

class CRouter 
{
public:
    static void Register(hv::HttpService* router);
};

class CHandler 
{
public:
    static int preProcessor(HttpRequest* req, HttpResponse* resp);
    static int postProcessor(HttpRequest* req, HttpResponse* resp);
    static int errorHandler(const HttpContextPtr& ctx);

    static int router(HttpRequest* req, HttpResponse* resp);
    static int metrics(HttpRequest* req, HttpResponse* resp);

private:
    static int initEnv(HttpRequest* req, HttpResponse* resp);
    static int addCmd(HttpRequest* req, HttpResponse* resp);
    static int runCmd(HttpRequest* req, HttpResponse* resp);
    static int getAmp(HttpRequest* req, HttpResponse* resp);
    static int getProb(HttpRequest* req, HttpResponse* resp);
    static int getState(HttpRequest* req, HttpResponse* resp);
    static int releaseEnv(HttpRequest* req, HttpResponse* resp);
    static int getTask(HttpRequest* req, HttpResponse* resp);
    static int getMeasure(HttpRequest* req, HttpResponse* resp);
    static int applyQft(HttpRequest* req, HttpResponse* resp);
    static int getEPauli(HttpRequest* req, HttpResponse* resp);
    static int getEPauliSum(HttpRequest* req, HttpResponse* resp);

    static int response_status(HttpResponse* resp, int code = 200, const char* message = NULL) {
        if (message == NULL) message = http_status_str((enum http_status)code);
        resp->Set("code", code);
        resp->Set("msg", message);
        return code;
    }
    static int response_status(const HttpResponseWriterPtr& writer, int code = 200, const char* message = NULL) {
        response_status(writer->response.get(), code, message);
        writer->End();
        return code;
    }
    static int response_status(const HttpContextPtr& ctx, int code = 200, const char* message = NULL) {
        response_status(ctx->response.get(), code, message);
        ctx->send();
        return code;
    }
};

#endif 
