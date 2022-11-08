#include "common/qulog.h"
#include "comm/errorcode.h"
#include "requestmanager.h"
#include "router.h"
#include "common/Singleton.h"
#include "metrics/metrics.h"

void CRouter::Register(hv::HttpService* router)
{
    router->preprocessor = CHandler::preProcessor;
    router->postprocessor = CHandler::postProcessor;
    router->errorHandler = CHandler::errorHandler;

    router->POST("/request", CHandler::router);
    router->GET("/metrics", CHandler::metrics);
}

int CHandler::preProcessor(HttpRequest* req, HttpResponse* resp) 
{
    //设置跨域
    resp->headers["Access-Control-Allow-Origin"] = "*";
    if (req->method == HTTP_OPTIONS) 
    {
        resp->headers["Access-Control-Allow-Origin"] = req->GetHeader("Origin", "*");
        resp->headers["Access-Control-Allow-Methods"] = req->GetHeader("Access-Control-Request-Method", "OPTIONS, HEAD, GET, POST, PUT, DELETE, PATCH");
        resp->headers["Access-Control-Allow-Headers"] = req->GetHeader("Access-Control-Request-Headers", "Content-Type");
        return HTTP_STATUS_NO_CONTENT;
    }

    req->ParseBody();
    resp->content_type = APPLICATION_JSON;
    return HTTP_STATUS_UNFINISHED;
}

int CHandler::postProcessor(HttpRequest* req, HttpResponse* resp) 
{
    return resp->status_code;
}

int CHandler::errorHandler(const HttpContextPtr& ctx) {
    int error_code = ctx->response->status_code;
    LOG(ERROR) << "errorHandler(error_code:" << error_code << ").";
    return response_status(ctx, error_code);
}

int CHandler::router(HttpRequest* req, HttpResponse* resp)
{
    LOG(INFO) << "router(req:" << req->Body() << ").";

    std::shared_ptr<ReqHead> headreq = std::make_shared<ReqHead>();
    bool rnt = headreq->decodehead(req->Body());
    if (true == rnt)
    {
        if (headreq->cmd == CMD_STR_INITENV)
        {
            initEnv(req, resp);
        }
        else if (headreq->cmd == CMD_STR_ADDCMD)
        {
            addCmd(req, resp);
        }
        else if (headreq->cmd == CMD_STR_RUNCMD)
        {
            runCmd(req, resp);
        }
        else if (headreq->cmd == CMD_STR_GETAMP)
        {
            getAmp(req, resp);
        }
        else if (headreq->cmd == CMD_STR_GETPROB)
        {
            getProb(req, resp);
        }
        else if (headreq->cmd == CMD_STR_GETSTATE)
        {
            getState(req, resp);
        }
        else if (headreq->cmd == CMD_STR_RELEASEENV)
        {
            releaseEnv(req, resp);
        }
        else if (headreq->cmd == CMD_STR_GETTASK)
        {
            getTask(req, resp);
        }
        else if (headreq->cmd == CMD_STR_GETMEASURE)
        {
            getMeasure(req, resp);
        }
        else if (headreq->cmd == CMD_STR_APPLYQFT)
        {
            applyQft(req, resp);
        }
        else if (headreq->cmd == CMD_STR_GETPAULI)
        {
            getEPauli(req, resp);
        }
        else if (headreq->cmd == CMD_STR_GETPAULISUM)
        {
            getEPauliSum(req, resp);
        }

        LOG(INFO) << "router(resp:" << resp->Body() << ").";
        return HTTP_STATUS_OK;
    }

    LOG(ERROR) << "router decode failed(req:" << req->Body() << ").";
    std::shared_ptr<RespHead> headresp = std::make_shared<RespHead>();
    headresp->setcode("", COMMON_PRASE_ERROR);
    std::string respstr = "";
    headresp->encodehead(respstr);
    resp->SetBody(respstr);
    resp->SetHeader("Content-Type", "application/json");
    return HTTP_STATUS_OK;
}

int CHandler::initEnv(HttpRequest* req, HttpResponse* resp)
{
    std::shared_ptr<InitEnvReq> envreq = std::make_shared<InitEnvReq>();
    std::shared_ptr<InitEnvResp> envresp = std::make_shared<InitEnvResp>();

    bool rnt = envreq->decode(req->Body());
    if (false == rnt)
    {
        LOG(ERROR) << "initEnv decode failed(req:" << envreq->getStr() << ").";
        envresp->setcode(envreq->flowid, COMMON_PRASE_ERROR);
    }
    else
    {
        SINGLETON(CReqManager)->initEnv(envreq, envresp);
    }

    std::string respstr = "";
    envresp->encode(respstr);
    resp->SetBody(respstr);
    resp->SetHeader("Content-Type", "application/json");

    return HTTP_STATUS_OK;
}

int CHandler::addCmd(HttpRequest* req, HttpResponse* resp)
{
    std::shared_ptr<AddGateCmdReq> addreq = std::make_shared<AddGateCmdReq>();
    std::shared_ptr<AddGateCmdResp> addresp = std::make_shared<AddGateCmdResp>();

    bool rnt = addreq->decode(req->Body());
    if (false == rnt)
    {
        LOG(ERROR) << "addCmd decode failed(req:" << addreq->getStr() << ").";
        addresp->setcode(addreq->flowid, COMMON_PRASE_ERROR);
    }
    else
    {
        SINGLETON(CReqManager)->addCmd(addreq, addresp);
    }

    std::string respstr = "";
    addresp->encode(respstr);
    resp->SetBody(respstr);
    resp->SetHeader("Content-Type", "application/json");

    return HTTP_STATUS_OK;
}

int CHandler::runCmd(HttpRequest* req, HttpResponse* resp)
{   
    std::shared_ptr<RunGateCmdReq> runreq = std::make_shared<RunGateCmdReq>();
    std::shared_ptr<RunGateCmdResp> runresp = std::make_shared<RunGateCmdResp>();

    bool rnt = runreq->decode(req->Body());
    if (false == rnt)
    {
        LOG(ERROR) << "runCmd decode failed(req:" << runreq->getStr() << ").";
        runresp->setcode(runreq->flowid, COMMON_PRASE_ERROR);
    }
    else
    {
        SINGLETON(CReqManager)->runCmd(runreq, runresp);
    }

    std::string respstr = "";
    runresp->encode(respstr);
    resp->SetBody(respstr);
    resp->SetHeader("Content-Type", "application/json");

    return HTTP_STATUS_OK;
}

int CHandler::getAmp(HttpRequest* req, HttpResponse* resp)
{
    std::shared_ptr<GetAmpReq> getreq = std::make_shared<GetAmpReq>();
    std::shared_ptr<GetAmpResp> getresp = std::make_shared<GetAmpResp>();

    bool rnt = getreq->decode(req->Body());
    if (false == rnt)
    {
        LOG(ERROR) << "getAmp decode failed(req:" << getreq->getStr() << ").";
        getresp->setcode(getreq->flowid, COMMON_PRASE_ERROR);
    }
    else
    {
        SINGLETON(CReqManager)->getAmp(getreq, getresp);
    }

    std::string respstr = "";
    getresp->encode(respstr);
    resp->SetBody(respstr);
    resp->SetHeader("Content-Type", "application/json");

    return HTTP_STATUS_OK;
}

int CHandler::getProb(HttpRequest* req, HttpResponse* resp)
{
    std::shared_ptr<GetProbReq> getreq = std::make_shared<GetProbReq>();
    std::shared_ptr<GetProbResp> getresp = std::make_shared<GetProbResp>();

    bool rnt = getreq->decode(req->Body());
    if (false == rnt)
    {
        LOG(ERROR) << "getProb decode failed(req:" << getreq->getStr() << ").";
        getresp->setcode(getreq->flowid, COMMON_PRASE_ERROR);
    }
    else
    {
        SINGLETON(CReqManager)->getProb(getreq, getresp);
    }

    std::string respstr = "";
    getresp->encode(respstr);
    resp->SetBody(respstr);
    resp->SetHeader("Content-Type", "application/json");

    return HTTP_STATUS_OK;
}

int CHandler::getState(HttpRequest* req, HttpResponse* resp)
{
    std::shared_ptr<GetStateReq> getreq = std::make_shared<GetStateReq>();
    std::shared_ptr<GetStateResp> getresp = std::make_shared<GetStateResp>();

    bool rnt = getreq->decode(req->Body());
    if (false == rnt)
    {
        LOG(ERROR) << "getState decode failed(req:" << getreq->getStr() << ").";
        getresp->setcode(getreq->flowid, COMMON_PRASE_ERROR);
    }
    else
    {
        SINGLETON(CReqManager)->getState(getreq, getresp);
    }

    std::string respstr = "";
    getresp->encode(respstr);
    resp->SetBody(respstr);
    resp->SetHeader("Content-Type", "application/json");

    return HTTP_STATUS_OK;
}

int CHandler::releaseEnv(HttpRequest* req, HttpResponse* resp)
{
    std::shared_ptr<ReleaseEnvReq> releasereq = std::make_shared<ReleaseEnvReq>();
    std::shared_ptr<ReleaseEnvResp> releaseresp = std::make_shared<ReleaseEnvResp>();

    bool rnt = releasereq->decode(req->Body());
    if (false == rnt)
    {
        LOG(ERROR) << "releaseEnv decode failed(req:" << releasereq->getStr() << ").";
        releaseresp->setcode(releasereq->flowid, COMMON_PRASE_ERROR);
    }
    else
    {
        SINGLETON(CReqManager)->releaseEnv(releasereq, releaseresp);
    }

    std::string respstr = "";
    releaseresp->encode(respstr);
    resp->SetBody(respstr);
    resp->SetHeader("Content-Type", "application/json");

    return HTTP_STATUS_OK;
}

int CHandler::getTask(HttpRequest* req, HttpResponse* resp)
{
    std::shared_ptr<GetTaskReq> getreq = std::make_shared<GetTaskReq>();
    std::shared_ptr<GetTaskResp> getresp = std::make_shared<GetTaskResp>();

    bool rnt = getreq->decode(req->Body());
    if (false == rnt)
    {
        LOG(ERROR) << "getTask decode failed(req:" << getreq->getStr() << ").";
        getresp->setcode(getreq->flowid, COMMON_PRASE_ERROR);
    }
    else
    {
        SINGLETON(CReqManager)->getTask(getreq, getresp);
    }

    std::string respstr = "";
    getresp->encode(respstr);
    resp->SetBody(respstr);
    resp->SetHeader("Content-Type", "application/json");

    return HTTP_STATUS_OK;
}

int CHandler::getMeasure(HttpRequest* req, HttpResponse* resp)
{
    std::shared_ptr<GetMeasureReq> getreq = std::make_shared<GetMeasureReq>();
    std::shared_ptr<GetMeasureResp> getresp = std::make_shared<GetMeasureResp>();

    bool rnt = getreq->decode(req->Body());
    if (false == rnt)
    {
        LOG(ERROR) << "getMeasure decode failed(req:" << getreq->getStr() << ").";
        getresp->setcode(getreq->flowid, COMMON_PRASE_ERROR);
    }
    else
    {
        SINGLETON(CReqManager)->getMeasure(getreq, getresp);
    }

    std::string respstr = "";
    getresp->encode(respstr);
    resp->SetBody(respstr);
    resp->SetHeader("Content-Type", "application/json");

    return HTTP_STATUS_OK;
}

int CHandler::applyQft(HttpRequest* req, HttpResponse* resp)
{
    std::shared_ptr<ApplyQftReq> applyreq = std::make_shared<ApplyQftReq>();
    std::shared_ptr<ApplyQftResp> applyresp = std::make_shared<ApplyQftResp>();

    bool rnt = applyreq->decode(req->Body());
    if (false == rnt)
    {
        LOG(ERROR) << "applyQft decode failed(req:" << applyreq->getStr() << ").";
        applyresp->setcode(applyreq->flowid, COMMON_PRASE_ERROR);
    }
    else
    {
        SINGLETON(CReqManager)->applyQft(applyreq, applyresp);
    }

    std::string respstr = "";
    applyresp->encode(respstr);
    resp->SetBody(respstr);
    resp->SetHeader("Content-Type", "application/json");

    return HTTP_STATUS_OK;
}

int CHandler::getEPauli(HttpRequest* req, HttpResponse* resp)
{
    std::shared_ptr<GetEPauliReq> getreq = std::make_shared<GetEPauliReq>();
    std::shared_ptr<GetEPauliResp> getresp = std::make_shared<GetEPauliResp>();

    bool rnt = getreq->decode(req->Body());
    if (false == rnt)
    {
        LOG(ERROR) << "getEPauli decode failed(req:" << getreq->getStr() << ").";
        getresp->setcode(getreq->flowid, COMMON_PRASE_ERROR);
    }
    else
    {
        SINGLETON(CReqManager)->getEPauli(getreq, getresp);
    }

    std::string respstr = "";
    getresp->encode(respstr);
    resp->SetBody(respstr);
    resp->SetHeader("Content-Type", "application/json");

    return HTTP_STATUS_OK;
}

int CHandler::getEPauliSum(HttpRequest* req, HttpResponse* resp)
{
    std::shared_ptr<GetEPauliSumReq> getreq = std::make_shared<GetEPauliSumReq>();
    std::shared_ptr<GetEPauliSumResp> getresp = std::make_shared<GetEPauliSumResp>();

    bool rnt = getreq->decode(req->Body());
    if (false == rnt)
    {
        LOG(ERROR) << "getEPauliSum decode failed(req:" << getreq->getStr() << ").";
        getresp->setcode(getreq->flowid, COMMON_PRASE_ERROR);
    }
    else
    {
        SINGLETON(CReqManager)->getEPauliSum(getreq, getresp);
    }

    std::string respstr = "";
    getresp->encode(respstr);
    resp->SetBody(respstr);
    resp->SetHeader("Content-Type", "application/json");

    return HTTP_STATUS_OK;
}

int CHandler::metrics(HttpRequest* req, HttpResponse* resp)
{
    auto body = SINGLETON(CMetrics)->getMetricsStr();
    if (!body.empty())
    {
        resp->SetBody(body);
    }
    
	return HTTP_STATUS_OK;
}
