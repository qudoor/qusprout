#include <sstream>
#include <iostream>
#include <rapidjson/error/en.h>
#include <rapidjson/stringbuffer.h>
#include "struct.h"
#include "common/qulog.h"
#include "comm/errorcode.h"

using namespace rapidjson;

#define EncodeString(key, value) writer.Key(key); writer.String(value.c_str());
#define EncodeInt(key, value) writer.Key(key); writer.Int(value);
#define EncodeArraryString(key, value) { writer.Key(key); \
    writer.StartArray(); \
    for (auto temp : value) { writer.String(temp.c_str()); } \
    writer.EndArray(); }
#define EncodeArraryInt(key, value) { writer.Key(key); \
    writer.StartArray(); \
    for (auto temp : value) { writer.Int(temp); } \
    writer.EndArray(); }
#define EncodeArraryDouble(key, value) { writer.Key(key); \
    writer.StartArray(); \
    for (auto temp : value) { writer.Double(temp); } \
    writer.EndArray(); }
#define EncodeInt64(key, value) writer.Key(key); writer.Int64(value);
#define EncodeDouble(key, value) writer.Key(key); writer.Double(value);

#define DecodeString(key, tempdom, value, required) if (tempdom.HasMember(key) && tempdom[key].IsString()) { value = tempdom[key].GetString(); } else if (required) { LOG(ERROR) << "decode not find required key(key:" << key << ")."; return false; }
#define DecodeInt(key, tempdom, value, required) if (tempdom.HasMember(key) && tempdom[key].IsInt()) { value = tempdom[key].GetInt(); } else if (required) { LOG(ERROR) << "decode not find required key(key:" << key << ")."; return false; }
#define DecodeInt64(key, tempdom, value, required) if (tempdom.HasMember(key) && tempdom[key].IsInt64()) { value = tempdom[key].GetInt64(); } else if (required) { LOG(ERROR) << "decode not find required key(key:" << key << ")."; return false; }
#define DecodeArraryString(key, tempdom, value, required) if (tempdom.HasMember(key) && tempdom[key].IsArray()) { for (unsigned int i = 0; i < tempdom[key].Size(); ++i) { if (!tempdom[key][i].IsString()) { LOG(ERROR) << "decode type is not string(key:" << key << ")."; return false; } value.push_back(tempdom[key][i].GetString()); } } else if (required) { LOG(ERROR) << "decode not find required key(key:" << key << ")."; return false; }
#define DecodeArraryInt(key, tempdom, value, required) if (tempdom.HasMember(key) && tempdom[key].IsArray()) { for (unsigned int i = 0; i < tempdom[key].Size(); ++i) { if (!tempdom[key][i].IsInt()) { LOG(ERROR) << "decode type is not int(key:" << key << ")."; return false; } value.push_back(tempdom[key][i].GetInt()); } } else if (required) { LOG(ERROR) << "decode not find required key(key:" << key << ")."; return false; }
#define DecodeArraryDouble(key, tempdom, value, required) if (tempdom.HasMember(key) && tempdom[key].IsArray()) { for (unsigned int i = 0; i < tempdom[key].Size(); ++i) { if (!tempdom[key][i].IsDouble()) { LOG(ERROR) << "decode type is not double(key:" << key << ")."; return false; } value.push_back(tempdom[key][i].GetDouble()); } } else if (required) { LOG(ERROR) << "decode not find required key(key:" << key << ")."; return false; }
#define DecodeDouble(key, tempdom, value, required) if (tempdom.HasMember(key) && tempdom[key].IsDouble()) { value = tempdom[key].GetDouble(); } else if (required) { LOG(ERROR) << "decode not find required key(key:" << key << ")."; return false; }

#define PraseReqJson() Document dom; \
    if (dom.Parse(buf.c_str()).HasParseError()) { LOG(ERROR) << "prase req failed(error:" << GetParseError_En(dom.GetParseError()) << ")."; return false; } \
    if (!ReqHead::decode(dom)) { LOG(ERROR) << "decode req head failed."; return false; }
#define PraseRespJson() Document dom; \
    if (dom.Parse(buf.c_str()).HasParseError()) { LOG(ERROR) << "prase resp failed(error:" << GetParseError_En(dom.GetParseError()) << ")."; return false; } \
    if (!RespHead::decode(dom)) { LOG(ERROR) << "decode resp head failed."; return false; }

void ReqHead::encode(Writer<StringBuffer>& writer)
{
    EncodeString("flowid", flowid);
    EncodeString("taskid", taskid);
    EncodeString("cmd", cmd);
}

bool ReqHead::decode(const Document& dom)
{
    DecodeString("flowid", dom, flowid, true);
    DecodeString("taskid", dom, taskid, true);
    DecodeString("cmd", dom, cmd, true);

    return true;
}

std::string ReqHead::getStr()
{
    std::ostringstream ss;
    ss << "flowid:" << flowid
        << ";taskid:" << taskid
        << ";cmd:" << cmd
        << ";";

    return ss.str();
}

bool ReqHead::decodehead(const std::string& buf)
{
    PraseReqJson();
    decode(dom);

    return true;
}

void RespHead::encode(Writer<StringBuffer>& writer)
{
    EncodeString("flowid", flowid);
    EncodeInt("code", code);
    EncodeString("msg", msg);
}

bool RespHead::decode(const Document& dom)
{
    DecodeString("flowid", dom, flowid, true);
    DecodeInt("code", dom, code, true);
    DecodeString("msg", dom, msg, true);

    return true;
}

std::string RespHead::getStr()
{
    std::ostringstream ss;
    ss << "flowid:" << flowid
        << ";code:" << code
        << ";msg:" << msg
        << ";";

    return ss.str();
}

void RespHead::setcode(const std::string& flowid, const int code)
{
    this->flowid = flowid;
    this->code = code;
    this->msg = g_errormsg[code];
}

void RespHead::encodehead(std::string& buf)
{
    StringBuffer bufstream;
	Writer<StringBuffer> writer(bufstream);
 
	writer.StartObject();
    {
        encode(writer);
    }
    writer.EndObject();

    buf = bufstream.GetString();
}

void MeasureInfo::encode(Writer<StringBuffer>& writer)
{
    EncodeInt("target", target);
    EncodeInt("value", value);
}

bool MeasureInfo::decode(const Value& dom)
{
    DecodeInt("target", dom, target, true);
    DecodeInt("value", dom, value, true);

    return true;
}

std::string MeasureInfo::getStr()
{
    std::ostringstream ss;
    ss << "target:" << target
        << ";value:" << value
        << ";";

    return ss.str();
}

void OutcomeInfo::encode(Writer<StringBuffer>& writer)
{
    EncodeString("bitstr", bitstr);
    EncodeInt("count", count);
}

bool OutcomeInfo::decode(const Value& dom)
{
    DecodeString("bitstr", dom, bitstr, true);
    DecodeInt("count", dom, count, true);

    return true;
}

std::string OutcomeInfo::getStr()
{
    std::ostringstream ss;
    ss << "bitstr:" << bitstr
        << ";count:" << count
        << ";";

    return ss.str();
}

void InitParam::encode(rapidjson::Writer<rapidjson::StringBuffer>& writer)
{
    EncodeInt("qubits", qubits);
    EncodeInt("density", density);
    EncodeInt("exec_type", exec_type);
    EncodeInt("async", async);
}

bool InitParam::decode(const rapidjson::Value& dom)
{
    DecodeInt("qubits", dom, qubits, false);
    DecodeInt("density", dom, density, false);
    DecodeInt("exec_type", dom, exec_type, false);
    DecodeInt("async", dom, async, false);

    return true;
}

std::string InitParam::getStr()
{
    std::ostringstream ss;
    ss << "qubits:" << qubits
        << ";density:" << density
        << ";exec_type:" << exec_type
        << ";async:" << async
        << ";";

    return ss.str();
}

void InitEnvReq::encode(std::string& buf)
{
    cmd = CMD_STR_INITENV;

    StringBuffer bufstream;
	Writer<StringBuffer> writer(bufstream);
 
	writer.StartObject();
    {
        ReqHead::encode(writer);

        writer.Key("params");
        writer.StartObject();
        {
            params.encode(writer);
        }
        writer.EndObject();
    }
    writer.EndObject();

    buf = bufstream.GetString();
}

bool InitEnvReq::decode(const std::string& buf)
{
    PraseReqJson();
        
    if (!dom.HasMember("params") || !dom["params"].IsObject())
    {
        LOG(ERROR) << "decode not find required key(key:params).";
        return false;
    }

    const rapidjson::Value& paramsdom = dom["params"];
    if (!params.decode(paramsdom))
    {
        LOG(ERROR) << "decode params failed.";
        return false;
    }

    return true;
}

std::string InitEnvReq::getStr()
{
    std::ostringstream ss;
    ss << "InitEnvReq:" << ReqHead::getStr()
        << params.getStr();

    return ss.str();
}

void InitEnvResp::encode(std::string& buf)
{
    StringBuffer bufstream;
	Writer<StringBuffer> writer(bufstream);
 
	writer.StartObject();
    {
        RespHead::encode(writer);
    }
    writer.EndObject();

    buf = bufstream.GetString();
}

bool InitEnvResp::decode(const std::string& buf)
{
    PraseRespJson();

    return true;
}

std::string InitEnvResp::getStr()
{
    std::ostringstream ss;
    ss << "InitEnvResp:" << RespHead::getStr();

    return ss.str();
}

void GateCmd::encode(rapidjson::Writer<rapidjson::StringBuffer>& writer)
{
    EncodeString("gate", gate);
    EncodeArraryInt("controls", controls);
    EncodeArraryInt("targets", targets);
    EncodeArraryDouble("rotations", rotations);
    EncodeString("qasmdef", qasmdef);
    EncodeInt("inverse", inverse);
}

bool GateCmd::decode(const rapidjson::Value& dom)
{
    DecodeString("gate", dom, gate, true);
    DecodeArraryInt("controls", dom, controls, false);
    DecodeArraryInt("targets", dom, targets, false);
    DecodeArraryDouble("rotations", dom, rotations, false);
    DecodeString("qasmdef", dom, qasmdef, true);
    DecodeInt("inverse", dom, inverse, false);

    return true;
}

std::string GateCmd::getStr()
{
    std::ostringstream ss;
    ss << "gate:" << gate
        << ";controls:[";
    for (auto temp : controls) 
    {
        ss << temp << ";";
    }
    ss << "];targets:[";
    for (auto temp : targets) 
    {
        ss << temp << ";";
    }
    ss << "];rotations:[";
    for (auto temp : rotations) 
    {
        ss << temp << ";";
    }
    ss << "];"
        << ";qasmdef:" << qasmdef
        << ";inverse:" << inverse;

    return ss.str();
}

void GateCmdParam::encode(rapidjson::Writer<rapidjson::StringBuffer>& writer)
{
    EncodeInt("final", final);

    writer.Key("circuits");
    writer.StartArray();
    {
        for (auto temp : circuits)
        {
            writer.StartObject();
            {
                temp.encode(writer);
            }
            writer.EndObject();
        }
    }
    writer.EndArray();
}

bool GateCmdParam::decode(const rapidjson::Value& dom)
{
    DecodeInt("final", dom, final, false);

    if (dom.HasMember("circuits") && dom["circuits"].IsArray())
    {
        const rapidjson::Value& circuitdom = dom["circuits"];
        for (unsigned int i = 0; i < circuitdom.Size(); ++i) 
        {
            GateCmd temp;
            const rapidjson::Value& result = circuitdom[i];
            if (!temp.decode(result))
            {
                return false;
            }
            circuits.push_back(temp);
        }
    }

    return true;
}

std::string GateCmdParam::getStr()
{
    std::ostringstream ss;
    ss << "final:" << final
        << ";circuits:[";
    for (auto temp : circuits) 
    {
        ss << temp.getStr() << ";";
    }
    ss << "];";

    return ss.str();
}

void AddGateCmdReq::encode(std::string& buf)
{
    cmd = CMD_STR_ADDCMD;

    StringBuffer bufstream;
	Writer<StringBuffer> writer(bufstream);
 
	writer.StartObject();
    {
        ReqHead::encode(writer);

        writer.Key("params");
        writer.StartObject();
        {
            params.encode(writer);
        }
        writer.EndObject();
    }
    writer.EndObject();

    buf = bufstream.GetString();
}

bool AddGateCmdReq::decode(const std::string& buf)
{
    PraseReqJson();

    if (!dom.HasMember("params") || !dom["params"].IsObject())
    {
        LOG(ERROR) << "decode not find required key(key:params).";
        return false;
    }

    const rapidjson::Value& paramsdom = dom["params"];
    if (!params.decode(paramsdom))
    {
        LOG(ERROR) << "decode params failed.";
        return false;
    }

    return true;
}

std::string AddGateCmdReq::getStr()
{
    std::ostringstream ss;
    ss << "AddGateCmdReq:" << ReqHead::getStr()
        << params.getStr();

    return ss.str();
}

void AddGateCmdResp::encode(std::string& buf)
{
    StringBuffer bufstream;
	Writer<StringBuffer> writer(bufstream);
 
	writer.StartObject();
    {
        RespHead::encode(writer);
    }
    writer.EndObject();

    buf = bufstream.GetString();
}

bool AddGateCmdResp::decode(const std::string& buf)
{
    PraseRespJson();

    return true;
}

std::string AddGateCmdResp::getStr()
{
    std::ostringstream ss;
    ss << "AddGateCmdResp:" << RespHead::getStr();

    return ss.str();
}

void RunGateCmdReq::encode(std::string& buf)
{
    cmd = CMD_STR_RUNCMD;

    StringBuffer bufstream;
	Writer<StringBuffer> writer(bufstream);
 
	writer.StartObject();
    {
        ReqHead::encode(writer);

        writer.Key("params");
        writer.StartObject();
        {
            EncodeInt("shots", shots);
        }
        writer.EndObject();
    }
    writer.EndObject();

    buf = bufstream.GetString();
}

bool RunGateCmdReq::decode(const std::string& buf)
{
    PraseReqJson();
        
    if (dom.HasMember("params") && dom["params"].IsObject())
    {
        const rapidjson::Value& paramsdom = dom["params"];
        DecodeInt("shots", paramsdom, shots, false);
    }

    return true;
}

std::string RunGateCmdReq::getStr()
{
    std::ostringstream ss;
    ss << "RunGateCmdReq:" << ReqHead::getStr()
        << ";shots:" << shots
        << ";";

    return ss.str();
}

void RunGateCmdResp::encode(std::string& buf)
{
    StringBuffer bufstream;
	Writer<StringBuffer> writer(bufstream);
 
	writer.StartObject();
    {
        RespHead::encode(writer);

        writer.Key("data");
        writer.StartObject();
        {
            writer.Key("measures");
            writer.StartArray();
            {
                for (auto temp : measures)
                {
                    writer.StartObject();
                    {
                        temp.encode(writer);
                    }
                    writer.EndObject();
                }
            }
            writer.EndArray();

            writer.Key("outcomes");
            writer.StartArray();
            {
                for (auto temp : outcomes)
                {
                    writer.StartObject();
                    {
                        temp.encode(writer);
                    }
                    writer.EndObject();
                }
            }
            writer.EndArray();
        }
        writer.EndObject();
    }
    writer.EndObject();

    buf = bufstream.GetString();
}

bool RunGateCmdResp::decode(const std::string& buf)
{
    PraseRespJson();

    if (dom.HasMember("data") && dom["data"].IsObject())
    {
        const rapidjson::Value& datadom = dom["data"];
        if (datadom.HasMember("measures") && datadom["measures"].IsArray())
        {
            const rapidjson::Value& measureslist = datadom["measures"];
            for (unsigned int i = 0; i < measureslist.Size(); ++i) 
            {
                MeasureInfo temp;
                const rapidjson::Value& result = measureslist[i];
                if (!temp.decode(result))
                {
                    return false;
                }
                measures.push_back(temp);
			}
        }

        if (datadom.HasMember("outcomes") && datadom["outcomes"].IsArray())
        {
            const rapidjson::Value& outcomelist = datadom["outcomes"];
            for (unsigned int i = 0; i < outcomelist.Size(); ++i) 
            {
                OutcomeInfo temp;
                const rapidjson::Value& result = outcomelist[i];
                if (!temp.decode(result))
                {
                    return false;
                }
                outcomes.push_back(temp);
			}
        }
    }

    return true;
}

std::string RunGateCmdResp::getStr()
{
    std::ostringstream ss;
    ss << "RunGateCmdResp:" << RespHead::getStr()
        << ";measures:[";
    for (auto temp : measures)
    {
        ss << temp.getStr() << ";";
    }
    ss << "];outcomes:[";
    for (auto temp : outcomes)
    {
        ss << temp.getStr() << ";";
    }
    ss << "];";

    return ss.str();
}

void GetAmpReq::encode(std::string& buf)
{
    cmd = CMD_STR_GETAMP;

    StringBuffer bufstream;
	Writer<StringBuffer> writer(bufstream);
 
	writer.StartObject();
    {
        ReqHead::encode(writer);

        writer.Key("params");
        writer.StartObject();
        {
            EncodeArraryInt("indexs", indexs);
        }
        writer.EndObject();
    }
    writer.EndObject();

    buf = bufstream.GetString();
}

bool GetAmpReq::decode(const std::string& buf)
{
    PraseReqJson();
        
    if (dom.HasMember("params") && dom["params"].IsObject())
    {
        const rapidjson::Value& paramsdom = dom["params"];
        DecodeArraryInt("indexs", paramsdom, indexs, false);
    }

    return true;
}

std::string GetAmpReq::getStr()
{
    std::ostringstream ss;
    ss << "GetAmpReq:" << ReqHead::getStr()
        << ";indexs:[";
    for (auto temp : indexs)
    {
        ss << temp << ";";
    }
    ss << "];";

    return ss.str();
}

void GetAmpResp::encode(std::string& buf)
{
    StringBuffer bufstream;
	Writer<StringBuffer> writer(bufstream);
 
	writer.StartObject();
    {
        RespHead::encode(writer);

        writer.Key("data");
        writer.StartObject();
        {
            EncodeArraryDouble("amps", amps);
        }
        writer.EndObject();
    }
    writer.EndObject();

    buf = bufstream.GetString();
}

bool GetAmpResp::decode(const std::string& buf)
{
    PraseRespJson();

    if (dom.HasMember("data") && dom["data"].IsObject())
    {
        const rapidjson::Value& datadom = dom["data"];
        DecodeArraryDouble("amps", datadom, amps, false);
    }

    return true;
}

std::string GetAmpResp::getStr()
{
    std::ostringstream ss;
    ss << "GetAmpResp:" << RespHead::getStr()
        << ";amps:[";
    for (auto temp : amps)
    {
        ss << temp << ";";
    }
    ss << "];";

    return ss.str();
}

void GetProbReq::encode(std::string& buf)
{
    cmd = CMD_STR_GETPROB;

    StringBuffer bufstream;
	Writer<StringBuffer> writer(bufstream);
 
	writer.StartObject();
    {
        ReqHead::encode(writer);

        writer.Key("params");
        writer.StartObject();
        {
            EncodeArraryInt("targets", targets);
        }
        writer.EndObject();
    }
    writer.EndObject();

    buf = bufstream.GetString();
}

bool GetProbReq::decode(const std::string& buf)
{
    PraseReqJson();
        
    if (dom.HasMember("params") && dom["params"].IsObject())
    {
        const rapidjson::Value& paramsdom = dom["params"];
        DecodeArraryInt("targets", paramsdom, targets, false);
    }

    return true;
}

std::string GetProbReq::getStr()
{
    std::ostringstream ss;
    ss << "GetProbReq:" << ReqHead::getStr()
        << ";targets:[";
    for (auto temp : targets)
    {
        ss << temp << ";";
    }
    ss << "];";

    return ss.str();
}

void GetProbResp::encode(std::string& buf)
{
    StringBuffer bufstream;
	Writer<StringBuffer> writer(bufstream);
 
	writer.StartObject();
    {
        RespHead::encode(writer);

        writer.Key("data");
        writer.StartObject();
        {
            EncodeArraryDouble("outcomes", outcomes);
        }
        writer.EndObject();
    }
    writer.EndObject();

    buf = bufstream.GetString();
}

bool GetProbResp::decode(const std::string& buf)
{
    PraseRespJson();

    if (dom.HasMember("data") && dom["data"].IsObject())
    {
        const rapidjson::Value& datadom = dom["data"];
        DecodeArraryDouble("outcomes", datadom, outcomes, false);
    }

    return true;
}

std::string GetProbResp::getStr()
{
    std::ostringstream ss;
    ss << "GetProbResp:" << RespHead::getStr()
        << ";outcomes:[";
    for (auto temp : outcomes)
    {
        ss << temp << ";";
    }
    ss << "];";

    return ss.str();
}

void GetStateReq::encode(std::string& buf)
{
    cmd = CMD_STR_GETSTATE;

    StringBuffer bufstream;
	Writer<StringBuffer> writer(bufstream);
 
	writer.StartObject();
    {
        ReqHead::encode(writer);
    }
    writer.EndObject();

    buf = bufstream.GetString();
}

bool GetStateReq::decode(const std::string& buf)
{
    PraseReqJson();

    return true;
}

std::string GetStateReq::getStr()
{
    std::ostringstream ss;
    ss << "GetStateReq:" << ReqHead::getStr();

    return ss.str();
}

void GetStateResp::encode(std::string& buf)
{
    StringBuffer bufstream;
	Writer<StringBuffer> writer(bufstream);
 
	writer.StartObject();
    {
        RespHead::encode(writer);

        writer.Key("data");
        writer.StartObject();
        {
            EncodeArraryString("states", states);
        }
        writer.EndObject();
    }
    writer.EndObject();

    buf = bufstream.GetString();
}

bool GetStateResp::decode(const std::string& buf)
{
    PraseRespJson();

    if (dom.HasMember("data") && dom["data"].IsObject())
    {
        const rapidjson::Value& datadom = dom["data"];
        DecodeArraryString("states", datadom, states, false);
    }

    return true;
}

std::string GetStateResp::getStr()
{
    std::ostringstream ss;
    ss << "GetStateResp:" << RespHead::getStr()
        << ";states:[";
    for (auto temp : states)
    {
        ss << temp << ";";
    }
    ss << "];";

    return ss.str();
}

void ReleaseEnvReq::encode(std::string& buf)
{
    cmd = CMD_STR_RELEASEENV;

    StringBuffer bufstream;
	Writer<StringBuffer> writer(bufstream);
 
	writer.StartObject();
    {
        ReqHead::encode(writer);
    }
    writer.EndObject();

    buf = bufstream.GetString();
}

bool ReleaseEnvReq::decode(const std::string& buf)
{
    PraseReqJson();

    return true;
}

std::string ReleaseEnvReq::getStr()
{
    std::ostringstream ss;
    ss << "ReleaseEnvReq:" << ReqHead::getStr();

    return ss.str();
}

void ReleaseEnvResp::encode(std::string& buf)
{
    StringBuffer bufstream;
	Writer<StringBuffer> writer(bufstream);
 
	writer.StartObject();
    {
        RespHead::encode(writer);
    }
    writer.EndObject();

    buf = bufstream.GetString();
}

bool ReleaseEnvResp::decode(const std::string& buf)
{
    PraseRespJson();

    return true;
}

std::string ReleaseEnvResp::getStr()
{
    std::ostringstream ss;
    ss << "ReleaseEnvResp:" << RespHead::getStr();

    return ss.str();
}

void GetTaskReq::encode(std::string& buf)
{
    cmd = CMD_STR_GETTASK;

    StringBuffer bufstream;
	Writer<StringBuffer> writer(bufstream);
 
	writer.StartObject();
    {
        ReqHead::encode(writer);
    }
    writer.EndObject();

    buf = bufstream.GetString();
}

bool GetTaskReq::decode(const std::string& buf)
{
    PraseReqJson();

    return true;
}

std::string GetTaskReq::getStr()
{
    std::ostringstream ss;
    ss << "GetTaskReq:" << ReqHead::getStr();

    return ss.str();
}

void GetTaskResp::encode(std::string& buf)
{
    StringBuffer bufstream;
	Writer<StringBuffer> writer(bufstream);
 
	writer.StartObject();
    {
        RespHead::encode(writer);

        writer.Key("data");
        writer.StartObject();
        {
            EncodeInt("state", state);
        }
        writer.EndObject();
    }
    writer.EndObject();

    buf = bufstream.GetString();
}

bool GetTaskResp::decode(const std::string& buf)
{
    PraseRespJson();

    if (dom.HasMember("data") && dom["data"].IsObject())
    {
        const rapidjson::Value& datadom = dom["data"];
        DecodeInt("state", datadom, state, false);
    }

    return true;
}

std::string GetTaskResp::getStr()
{
    std::ostringstream ss;
    ss << "GetTaskResp:" << RespHead::getStr()
        << ";state" << state
        << ";";

    return ss.str();
}

void GetMeasureReq::encode(std::string& buf)
{
    cmd = CMD_STR_GETMEASURE;

    StringBuffer bufstream;
	Writer<StringBuffer> writer(bufstream);
 
	writer.StartObject();
    {
        ReqHead::encode(writer);

        writer.Key("params");
        writer.StartObject();
        {
            EncodeArraryInt("targets", targets);
        }
        writer.EndObject();
    }
    writer.EndObject();

    buf = bufstream.GetString();
}

bool GetMeasureReq::decode(const std::string& buf)
{
    PraseReqJson();
        
    if (dom.HasMember("params") && dom["params"].IsObject())
    {
        const rapidjson::Value& paramsdom = dom["params"];
        DecodeArraryInt("targets", paramsdom, targets, false);
    }

    return true;
}

std::string GetMeasureReq::getStr()
{
    std::ostringstream ss;
    ss << ReqHead::getStr()
        << ";targets:[";
    for (auto temp : targets)
    {
        ss << temp << ";";
    }
    ss << "];";

    return ss.str();
}

void GetMeasureResp::encode(std::string& buf)
{
    StringBuffer bufstream;
	Writer<StringBuffer> writer(bufstream);
 
	writer.StartObject();
    {
        RespHead::encode(writer);

        writer.Key("data");
        writer.StartObject();
        {
            writer.Key("measures");
            writer.StartArray();
            {
                for (auto temp : measures)
                {
                    writer.StartObject();
                    {
                        temp.encode(writer);
                    }
                    writer.EndObject();
                }
            }
            writer.EndArray();

            writer.Key("outcomes");
            writer.StartArray();
            {
                for (auto temp : outcomes)
                {
                    writer.StartObject();
                    {
                        temp.encode(writer);
                    }
                    writer.EndObject();
                }
            }
            writer.EndArray();
        }
        writer.EndObject();
    }
    writer.EndObject();

    buf = bufstream.GetString();
}

bool GetMeasureResp::decode(const std::string& buf)
{
    PraseRespJson();

    if (dom.HasMember("data") && dom["data"].IsObject())
    {
        const rapidjson::Value& datadom = dom["data"];
        if (datadom.HasMember("measures") && datadom["measures"].IsArray())
        {
            const rapidjson::Value& measureslist = datadom["measures"];
            for (unsigned int i = 0; i < measureslist.Size(); ++i) 
            {
                MeasureInfo temp;
                const rapidjson::Value& result = measureslist[i];
                if (!temp.decode(result))
                {
                    return false;
                }
                measures.push_back(temp);
			}
        }

        if (datadom.HasMember("outcomes") && datadom["outcomes"].IsArray())
        {
            const rapidjson::Value& outcomelist = datadom["outcomes"];
            for (unsigned int i = 0; i < outcomelist.Size(); ++i) 
            {
                OutcomeInfo temp;
                const rapidjson::Value& result = outcomelist[i];
                if (!temp.decode(result))
                {
                    return false;
                }
                outcomes.push_back(temp);
			}
        }
    }

    return true;
}

std::string GetMeasureResp::getStr()
{
    std::ostringstream ss;
    ss << "GetMeasureResp:" << RespHead::getStr()
        << ";measures:[";
    for (auto temp : measures)
    {
        ss << temp.getStr() << ";";
    }
    ss << "];outcomes:[";
    for (auto temp : outcomes)
    {
        ss << temp.getStr() << ";";
    }
    ss << "];";

    return ss.str();
}

void ApplyQftReq::encode(std::string& buf)
{
    cmd = CMD_STR_APPLYQFT;

    StringBuffer bufstream;
	Writer<StringBuffer> writer(bufstream);
 
	writer.StartObject();
    {
        ReqHead::encode(writer);

        writer.Key("params");
        writer.StartObject();
        {
            EncodeArraryInt("targets", targets);
        }
        writer.EndObject();
    }
    writer.EndObject();

    buf = bufstream.GetString();
}

bool ApplyQftReq::decode(const std::string& buf)
{
    PraseReqJson();
        
    if (dom.HasMember("params") && dom["params"].IsObject())
    {
        const rapidjson::Value& paramsdom = dom["params"];
        DecodeArraryInt("targets", paramsdom, targets, false);
    }

    return true;
}

std::string ApplyQftReq::getStr()
{
    std::ostringstream ss;
    ss << ReqHead::getStr()
        << ";targets:[";
    for (auto temp : targets)
    {
        ss << temp << ";";
    }
    ss << "];";

    return ss.str();
}

void ApplyQftResp::encode(std::string& buf)
{
    StringBuffer bufstream;
	Writer<StringBuffer> writer(bufstream);
 
	writer.StartObject();
    {
        RespHead::encode(writer);
    }
    writer.EndObject();

    buf = bufstream.GetString();
}

bool ApplyQftResp::decode(const std::string& buf)
{
    PraseRespJson();

    return true;
}

std::string ApplyQftResp::getStr()
{
    std::ostringstream ss;
    ss << "ApplyQftResp:" << RespHead::getStr();

    return ss.str();
}

void PauliInfo::encode(rapidjson::Writer<rapidjson::StringBuffer>& writer)
{
    EncodeInt("opertype", opertype);
    EncodeInt("target", target);
}

bool PauliInfo::decode(const rapidjson::Value& dom)
{
    DecodeInt("opertype", dom, opertype, true);
    DecodeInt("target", dom, target, true);

    return true;
}

std::string PauliInfo::getStr()
{
    std::ostringstream ss;
    ss << "opertype:" << opertype
        << ";target:" << target
        << ";";

    return ss.str();
}

void GetEPauliReq::encode(std::string& buf)
{
    cmd = CMD_STR_GETPAULI;

    StringBuffer bufstream;
	Writer<StringBuffer> writer(bufstream);
 
	writer.StartObject();
    {
        ReqHead::encode(writer);

        writer.Key("params");
        writer.StartObject();
        {
            writer.Key("paulis");
            writer.StartArray();
            {
                for (auto temp : paulis)
                {
                    writer.StartObject();
                    {
                        temp.encode(writer);
                    }
                    writer.EndObject();
                }
            }
            writer.EndArray();
        }
        writer.EndObject();
    }
    writer.EndObject();

    buf = bufstream.GetString();
}

bool GetEPauliReq::decode(const std::string& buf)
{
    PraseReqJson();
        
    if (dom.HasMember("params") && dom["params"].IsObject())
    {
        const rapidjson::Value& paramsdom = dom["params"];

        if (!paramsdom.HasMember("paulis") || !paramsdom["paulis"].IsArray())
        {
            return false;
        }

        const rapidjson::Value& paulilist = paramsdom["paulis"];
        for (unsigned int i = 0; i < paulilist.Size(); ++i) 
        {
            PauliInfo temp;
            const rapidjson::Value& result = paulilist[i];
            if (!temp.decode(result))
            {
                return false;
            }
            paulis.push_back(temp);
        }
    }

    return true;
}

std::string GetEPauliReq::getStr()
{
    std::ostringstream ss;
    ss << ReqHead::getStr()
        << ";paulis:[";
    for (auto temp : paulis)
    {
        ss << temp.getStr() << ";";
    }
    ss << "];";

    return ss.str();
}

void GetEPauliResp::encode(std::string& buf)
{
    StringBuffer bufstream;
	Writer<StringBuffer> writer(bufstream);
 
	writer.StartObject();
    {
        RespHead::encode(writer);

        writer.Key("data");
        writer.StartObject();
        {
            EncodeDouble("expect", expect);
        }
        writer.EndObject();
    }
    writer.EndObject();

    buf = bufstream.GetString();
}

bool GetEPauliResp::decode(const std::string& buf)
{
    PraseRespJson();

    if (dom.HasMember("data") && dom["data"].IsObject())
    {
        const rapidjson::Value& datadom = dom["data"];
        DecodeDouble("expect", datadom, expect, false);
    }

    return true;
}

std::string GetEPauliResp::getStr()
{
    std::ostringstream ss;
    ss << "GetEPauliResp:" << RespHead::getStr()
        << ";expect:" << expect
        << ";";

    return ss.str();
}

void GetEPauliSumReq::encode(std::string& buf)
{
    cmd = CMD_STR_GETPAULISUM;

    StringBuffer bufstream;
	Writer<StringBuffer> writer(bufstream);
 
	writer.StartObject();
    {
        ReqHead::encode(writer);

        writer.Key("params");
        writer.StartObject();
        {
            EncodeArraryInt("opertypes", opertypes);
            EncodeArraryDouble("terms", terms);
        }
        writer.EndObject();
    }
    writer.EndObject();

    buf = bufstream.GetString();
}

bool GetEPauliSumReq::decode(const std::string& buf)
{
    PraseReqJson();
        
    if (dom.HasMember("params") && dom["params"].IsObject())
    {
        const rapidjson::Value& paramsdom = dom["params"];
        DecodeArraryInt("opertypes", paramsdom, opertypes, true);
        DecodeArraryDouble("terms", paramsdom, terms, true);
    }

    return true;
}

std::string GetEPauliSumReq::getStr()
{
    std::ostringstream ss;
    ss << ReqHead::getStr()
        << ";opertypes:[";
    for (auto temp : opertypes)
    {
        ss << temp << ";";
    }
    ss << "];terms:[";
    for (auto temp : terms)
    {
        ss << temp << ";";
    }
    ss << "];";

    return ss.str();
}

void GetEPauliSumResp::encode(std::string& buf)
{
    StringBuffer bufstream;
	Writer<StringBuffer> writer(bufstream);
 
	writer.StartObject();
    {
        RespHead::encode(writer);

        writer.Key("data");
        writer.StartObject();
        {
            EncodeDouble("expect", expect);
        }
        writer.EndObject();
    }
    writer.EndObject();

    buf = bufstream.GetString();
}

bool GetEPauliSumResp::decode(const std::string& buf)
{
    PraseRespJson();

    if (dom.HasMember("data") && dom["data"].IsObject())
    {
        const rapidjson::Value& datadom = dom["data"];
        DecodeDouble("expect", datadom, expect, false);
    }

    return true;
}

std::string GetEPauliSumResp::getStr()
{
    std::ostringstream ss;
    ss << "GetEPauliSumResp:" << RespHead::getStr()
        << ";expect:" << expect
        << ";";

    return ss.str();
}
