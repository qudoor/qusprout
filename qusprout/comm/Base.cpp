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

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>
#include <thread>
#include <chrono>
#include <string.h>
#include <sys/time.h>
#include "Base.h"

CBase::CBase()
{

}

CBase::~CBase()
{

}

int CBase::getLocalPort() 
{
    unsigned short port = 0;
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) 
    {
        LOG(ERROR) << "socket failed(error:" << strerror(errno) << ").";
        return port;
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    //若port指定为0, 则调用bind时，系统会为其指定一个可用的端口号
    addr.sin_port = 0;        
    int ret = bind(fd, (struct sockaddr *) &addr, sizeof addr); 
    if (0 != ret) 
    {
        LOG(ERROR) << "bind failed(error:" << strerror(errno) << ").";
        return port;
    }

    struct sockaddr_in sockaddr;
    int len = sizeof(sockaddr);
    ret = getsockname(fd, (struct sockaddr *) &sockaddr, (socklen_t *) &len);
    if (0 != ret) 
    {
        LOG(ERROR) << "getsockname failed(error:" << strerror(errno) << ").";
        close(fd);
        return port;
    }

    //获取端口号
    port = ntohs(sockaddr.sin_port); 
 
    close(fd);
    return port;
}

//开机清理task
int CBase::killTask(const std::string processName)
{
    char pidbuf[1025] = {0};
    size_t pidbuflen = sizeof(pidbuf);
    size_t pidlen = pidbuflen - 1;

    int ret = -1;
    std::ostringstream os("");
    os << "ps aux | grep " << processName << " | grep -v \"grep\" | awk '{print $2}'";
    FILE* fp = popen(os.str().c_str(), "r");
    if (fp == NULL)
    {
        LOG(ERROR) << "popen failed(cmd:" << os.str() << ",error:" << strerror(errno) << ").";
        return -1;
    }

    do
    {
        char* p = (char*)pidbuf;
        memset(pidbuf, '\0', pidbuflen);
        ret = getdelim(&p, &pidlen, '\n', fp);
        if (ret < 0)
        {
            break;
        }

        pid_t pid = atoi(pidbuf);
        kill(pid, SIGKILL);
        LOG(ERROR) << "start kill sub process(processName:" << processName << ",pid:" << pid << ").";
    } while (ret >= 0);

    pclose(fp);
    return 0;
}

//等待指定端口监听完成
bool CBase::waitListenFinish(const int port, const int milliseconds)
{
    bool istrue = false;
    //休眠200毫秒，5次
    int sleepmill = 200, count = 5;
    if (milliseconds >= sleepmill*count)
    {
        count = milliseconds / sleepmill;
    }
    int currcount = count;
    while(milliseconds == 0 || currcount > 0)
    {
        --currcount;
        char pidbuf[1025] = {0};
        size_t pidbuflen = sizeof(pidbuf);
        size_t pidlen = pidbuflen - 1;

        int ret = -1;
        std::ostringstream os("");
        os << "lsof -i tcp:" << port << " | awk '{print $2}' | grep -v \"PID\"";
        FILE* fp = popen(os.str().c_str(), "r");
        if (fp == NULL)
        {
            LOG(ERROR) << "popen failed(cmd:" << os.str() << ",error:" << strerror(errno) << ").";
            std::this_thread::sleep_for(std::chrono::milliseconds(sleepmill));
            continue;
        }

        do
        {
            char* p = (char*)pidbuf;
            memset(pidbuf, '\0', pidbuflen);
            ret = getdelim(&p, &pidlen, '\n', fp);
            if (ret < 0)
            {
                break;
            }

            LOG(INFO) << "getline(port:" << port << ",pidbuf:" << pidbuf  << ").";

            if (isDigitStr(p))
            {
                pid_t pid = atoi(p);
                if (pid > 0)
                {
                    istrue = true;
                    break;
                }

            }
        } while (ret >= 0);

        pclose(fp);

        if (true == istrue)
        {
            LOG(INFO) << "waitListenFinish " << port << " start success";
            break;
        }
    }

    return istrue;
}

//tpc是否需要重连
bool CBase::isNeedReConnectCode(const apache::thrift::transport::TTransportException& e)
{
    if (e.getType() == apache::thrift::transport::TTransportException::TTransportExceptionType::CLIENT_DISCONNECT ||
        e.getType() == apache::thrift::transport::TTransportException::TTransportExceptionType::NOT_OPEN ||
        e.getType() == apache::thrift::transport::TTransportException::TTransportExceptionType::END_OF_FILE
    )
    {   
        return true;
    }

    return false;
}

//判断传入字符串是否全数字
bool CBase::isDigitStr(char* str)
{
    return (strspn(str, "0123456789") == strlen(str));
}

void setBase(BaseCode& base, const ErrCode::type& code, const std::string& msg)
{
    base.__set_code(code);
    if (msg.empty())
    {
        auto ptr = g_ecode_constants.ErrMsg.find(code);
        base.__set_msg(ptr->second);
    }
    else
    {
        base.__set_msg(msg);
    }
}

void ResourceData::reset()
{
    cpubytes = 0;
    gpubytes = 0;
}

ResourceData ResourceData::operator+=(const ResourceData& temp)
{
    cpubytes += temp.cpubytes;
    gpubytes += temp.gpubytes;
    return *this;
}

long long int getCurrMs()
{
    struct timeval time;
    gettimeofday(&time, NULL);
    return (time.tv_sec*1000 + time.tv_usec/1000);
}
