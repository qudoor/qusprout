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

#ifndef __H__STRUCT__H__
#define __H__STRUCT__H__

#include <string>
#include <vector>
#include <rapidjson/writer.h>
#include <rapidjson/document.h>

const std::string CMD_STR_INITENV = "initenv";
const std::string CMD_STR_ADDCMD = "addcmd";
const std::string CMD_STR_RUNCMD = "runcmd";
const std::string CMD_STR_GETAMP = "getamp";
const std::string CMD_STR_GETPROB = "getprob";
const std::string CMD_STR_GETSTATE = "getstate";
const std::string CMD_STR_RELEASEENV = "releaseenv";
const std::string CMD_STR_GETTASK = "gettask";
const std::string CMD_STR_GETMEASURE = "getmeasure";
const std::string CMD_STR_APPLYQFT = "applyqft";
const std::string CMD_STR_GETPAULI = "getpauli";
const std::string CMD_STR_GETPAULISUM = "getpaulisum";

//请求包头
struct ReqHead 
{
    //必填| 流水号
    std::string flowid{""}; 

    //必填| 会话id，同一会话保持一致            
    std::string taskid{""};

    //必填| 指令类型        
    std::string cmd{""}; 

    void encode(rapidjson::Writer<rapidjson::StringBuffer>& writer);
    bool decode(const rapidjson::Document& dom);
    std::string getStr();

    bool decodehead(const std::string& buf);
};

//响应包头
struct RespHead 
{
    //必填| 流水号
    std::string flowid{""}; 

    //必填| 返回码        
    int code{0};     

    //必填| 返回信息描述           
    std::string msg{""};        

    void encode(rapidjson::Writer<rapidjson::StringBuffer>& writer);
    bool decode(const rapidjson::Document& dom);
    std::string getStr();

    void setcode(const std::string& flowid, const int code);
    void encodehead(std::string& buf);
};

//测量结果
struct MeasureInfo
{
    //必填| 目标比特位
    int target{0};

    //必填| 测量的值（0或者1）
    int value{0};

    void encode(rapidjson::Writer<rapidjson::StringBuffer>& writer);
    bool decode(const rapidjson::Value& dom);
    std::string getStr();
};

//目标出现概率
struct OutcomeInfo
{
    //必填| 比特位组成的字符串，例如2个qubit时，那么有"00","01","10","11"的4个值
    std::string bitstr{""};

    //必填| 目标出现的次数
    int count{0};

    void encode(rapidjson::Writer<rapidjson::StringBuffer>& writer);
    bool decode(const rapidjson::Value& dom);
    std::string getStr();
};

struct InitParam
{
    //选填| 量子比特数量，必须>=0,为0需要等待所有指令接收完成才去执行量子操作
    int qubits{0};

    //选填| 是否是密度矩阵，0：否，1：是
    int density{0};

    //执行指令的方式，0：默认, 1：单个cpu执行, 2：多个cpu执行, 3：单个gpu执行
    int exec_type{0};

    void encode(rapidjson::Writer<rapidjson::StringBuffer>& writer);
    bool decode(const rapidjson::Value& dom);
    std::string getStr();  
};

//初始化量子环境
struct InitEnvReq : public ReqHead 
{
    InitParam params;

    void encode(std::string& buf);
    bool decode(const std::string& buf);
    std::string getStr();
};

struct InitEnvResp : public RespHead 
{
    void encode(std::string& buf);
    bool decode(const std::string& buf);
    std::string getStr();
};

struct GateCmd
{
    //必填| 量子门名称
    std::string gate{""};

    //选填| 操作的控制比特
    std::vector<int> controls;

    //选填| 操作的目标比特
    std::vector<int> targets;

    //选填| 旋转角度
    std::vector<double> rotations;

    //必填| qasm2.0描述
    std::string qasmdef{""};

    //选填| 是否执行逆操作，0：否，1：是
    int inverse{0};

    void encode(rapidjson::Writer<rapidjson::StringBuffer>& writer);
    bool decode(const rapidjson::Value& dom);
    std::string getStr();  
};

struct GateCmdParam
{
    //选填| 最后的线路
    int final{0};

    //选填| 数组，量子指令
    std::vector<GateCmd> circuits;

    void encode(rapidjson::Writer<rapidjson::StringBuffer>& writer);
    bool decode(const rapidjson::Value& dom);
    std::string getStr();  
};

//添加量子指令
struct AddGateCmdReq : public ReqHead 
{
    GateCmdParam params;
    
    void encode(std::string& buf);
    bool decode(const std::string& buf);
    std::string getStr();
};

struct AddGateCmdResp : public RespHead 
{
    void encode(std::string& buf);
    bool decode(const std::string& buf);
    std::string getStr();
};

//重复执行线路操作
struct RunGateCmdReq : public ReqHead 
{
    //选填| 重复shots次运行整个线路
    int shots{0};
    
    void encode(std::string& buf);
    bool decode(const std::string& buf);
    std::string getStr();
};

struct RunGateCmdResp : public RespHead 
{
    //选填| 测量结果
    std::vector<MeasureInfo> measures;

    //选填| 目标出现概率
    std::vector<OutcomeInfo> outcomes;

    void encode(std::string& buf);
    bool decode(const std::string& buf);
    std::string getStr();
};

//获取振幅
struct GetAmpReq : public ReqHead 
{
    //选填| 索引，取值范围：[0, 2^qubitnum)
    std::vector<int> indexs;
    
    void encode(std::string& buf);
    bool decode(const std::string& buf);
    std::string getStr();
};

struct GetAmpResp : public RespHead 
{
    //选填| 振幅
    std::vector<double> amps;

    void encode(std::string& buf);
    bool decode(const std::string& buf);
    std::string getStr();
};

//获取目标比特为0的概率
struct GetProbReq : public ReqHead 
{
    //选填| 目标比特位,取值范围：[0, qubitnum),单个比特是获取0的概率，多个比特是获取组合概率
    std::vector<int> targets;
    
    void encode(std::string& buf);
    bool decode(const std::string& buf);
    std::string getStr();
};

struct GetProbResp : public RespHead 
{
    //选填| 概率值
    std::vector<double> outcomes;

    void encode(std::string& buf);
    bool decode(const std::string& buf);
    std::string getStr();
};

//获取量子执行状态
struct GetStateReq : public ReqHead 
{
    void encode(std::string& buf);
    bool decode(const std::string& buf);
    std::string getStr();
};

struct GetStateResp : public RespHead 
{
    //选填| 每个状态位的实部和虚部的组成的字符串，格式位：real,imag，数量为2^qubitnum
    std::vector<std::string> states;

    void encode(std::string& buf);
    bool decode(const std::string& buf);
    std::string getStr();
};

//释放量子环境
struct ReleaseEnvReq : public ReqHead 
{
    void encode(std::string& buf);
    bool decode(const std::string& buf);
    std::string getStr();
};

struct ReleaseEnvResp : public RespHead 
{
    void encode(std::string& buf);
    bool decode(const std::string& buf);
    std::string getStr();
};

//获取任务的状态
struct GetTaskReq : public ReqHead 
{
    void encode(std::string& buf);
    bool decode(const std::string& buf);
    std::string getStr();
};

struct GetTaskResp : public RespHead 
{
    //必填| 任务状态，0：initial, 1：initialized, 2:queued, 3:running, 4:done, 5:error
    int state{0};

    void encode(std::string& buf);
    bool decode(const std::string& buf);
    std::string getStr();
};

//获取测量结果
struct GetMeasureReq : public ReqHead 
{
    //选填| 目标位
    std::vector<int> targets;

    void encode(std::string& buf);
    bool decode(const std::string& buf);
    std::string getStr();
};

struct GetMeasureResp : public RespHead 
{
    //选填| 测量结果
    std::vector<MeasureInfo> measures;

    //选填| 目标出现概率
    std::vector<OutcomeInfo> outcomes;

    void encode(std::string& buf);
    bool decode(const std::string& buf);
    std::string getStr();
};

//应用量子傅立叶变换
struct ApplyQftReq : public ReqHead 
{
    //选填| 目标位，为空时，对所有量子比特应用量子傅立叶变换
    std::vector<int> targets;

    void encode(std::string& buf);
    bool decode(const std::string& buf);
    std::string getStr();
};

struct ApplyQftResp : public RespHead 
{
    void encode(std::string& buf);
    bool decode(const std::string& buf);
    std::string getStr();
};

//获取泡利算子乘积的期望值
struct PauliInfo 
{
    //必填| 泡利算子操作类型，0：POT_PAULI_I，1：POT_PAULI_X，2：POT_PAULI_Y，3：POT_PAULI_Z
    int opertype{0};

    //必填| 目标比特位
    int target{0};

    void encode(rapidjson::Writer<rapidjson::StringBuffer>& writer);
    bool decode(const rapidjson::Value& dom);
    std::string getStr();
};

struct GetEPauliReq : public ReqHead 
{
    //选填| 期望值信息
    std::vector<PauliInfo> paulis;

    void encode(std::string& buf);
    bool decode(const std::string& buf);
    std::string getStr();
};

struct GetEPauliResp : public RespHead 
{
    //选填| 期望值
    double expect{0};

    void encode(std::string& buf);
    bool decode(const std::string& buf);
    std::string getStr();
};

//获取泡利算子乘积之和的期望值
struct GetEPauliSumReq : public ReqHead 
{
    //必填| 泡利算子操作类型，0：POT_PAULI_I，1：POT_PAULI_X，2：POT_PAULI_Y，3：POT_PAULI_Z
    std::vector<int> opertypes;

    //必填| 回归系数
    std::vector<double> terms;

    void encode(std::string& buf);
    bool decode(const std::string& buf);
    std::string getStr();
};

struct GetEPauliSumResp : public RespHead 
{
    //选填| 期望值
    double expect;

    void encode(std::string& buf);
    bool decode(const std::string& buf);
    std::string getStr();
};

#endif
