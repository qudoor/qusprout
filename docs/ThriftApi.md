
# QuSprout的prometheus监控配置文档
## 1 概述
文档对QuSprout的接口的定义、字段的描述、示例等进行了详细的说明。

## 2 接口
1、QuSprout采用Apache thrift定义接口语言定义接口文件，然后根据thrift命令生成*.cpp和*.h接口文件；

### 2.1 初始化量子环境
接口说明：
1.所有接口调用前，必须调用改接口，初始化量子执行环境，同一个任务的所有接口的id字段必须一致，id一样就是同一个任务；

2.释放量子环境必须调用run接口或者调用cancel接口；

请求说明：
| 序号 | 字段名称 | 类型 | 是否必填 | 说明 |
| ---- | ---- | ---- | ---- | ---- |
|1 |id  |string   |y  |任务id，同一任务保持一致 |
|2 |qubits  |int  |y  |申请的量子比特数量，必须>0 |
|3 |density  |int  |n  |是否是密度矩阵，0：否，1：是 |
|4 |exec_type  |int  |n  |执行指令的方式， 0、1：单cpu，2：多cpu的mpi，3：单gpu |
|5 |hosts  |arrary-string  |n  |执行任务的机器hostname列表，不填写，自动分配 |

响应说明：
| 序号 | 字段名称 | 类型 | 是否必填 | 说明 |
| ---- | ---- | ---- | ---- | ---- |
|1 |base  |object   |y  |返回码 |
|1.1 |code  |int   |y  |返回码，0：成功，其它值：参考ecode.thrift文件 |
|1.2 |msg  |string   |y  |返回信息描述 |

请求和响应数据（qusproutdata.thrift）定义：
```
//执行指令的方式
enum ExecCmdType {
    //默认值
    ExecTypeDefault = 0,

    //单cpu执行
    ExecTypeCpuSingle = 1,

    //多cpu的mpi执行
    ExecTypeCpuMpi = 2,

    //单gpu执行（暂时不支持）
    ExecTypeGpuSingle = 3
}

//任务初始化信息
struct InitQubitsReq {
    //任务id
    1: required string id

    //qubit数量
    2: required i32 qubits

    //密度矩阵
    3: optional bool density

    //执行指令的方式
    4: optional ExecCmdType exec_type

    //执行的host列表
    5: optional list<string> hosts
}

struct InitQubitsResp {
    //返回码
    1: required ecode.BaseCode base
}
```

接口文件（qusprout.thrift）定义：
```
service QuSproutServer {
    //初始化量子环境
    qusproutdata.InitQubitsResp initQubits(1:qusproutdata.InitQubitsReq req)
}
```

c++示例：
```
//连接QuSprout
std::shared_ptr<TSocket> socket = std::make_shared<TSocket>("127.0.0.1", 9091); 
std::shared_ptr<TTransport> transport = std::make_shared<TBufferedTransport>(socket);
std::shared_ptr<TProtocol> protocol = std::make_shared<TBinaryProtocol>(transport);
std::shared_ptr<TMultiplexedProtocol> quest = std::make_shared<TMultiplexedProtocol>(protocol, "QuSproutServer");
transport->open();
std::shared_ptr<QuSproutServerClient> client = std::make_shared<QuSproutServerClient>(quest);

std::ostringstream os("");
os << time(NULL);
auto id = os.str();

//初始化量子环境
const int qubitnum = 2;
InitQubitsReq initreq;
InitQubitsResp initresp;
initreq.__set_id(id);
initreq.__set_qubits(qubitnum);
initreq.__set_density(false);
initreq.__set_exec_type(ExecCmdType::Type::ExecTypeCpuSingle);
client->initQubits(initresp, initreq);

//释放量子环境
......
```

### 2.2 添加量子指令
接口说明：
1.添加量子门操作到量子线路中；

2.当量子门操作的最后一次时，final字段需改成true，代表添加线路完成；

请求说明：
| 序号 | 字段名称 | 类型 | 是否必填 | 说明 |
| ---- | ---- | ---- | ---- | ---- |
|1 |id  |string   |y  |任务id，同一任务保持一致 |
|2 |circuit  |object  |n  |量子指令 |
|2.1 |cmds  |arrary-object  |y  |指令列表 |
|2.1.1 |gate  |string  |y  |量子门名称，支持的操作：h、measure、p、cp、r、rx、ry、rz、rxx、ryy、rzz、x、y、z、s、t、sdg、tdg、sqrtx、sqrtswap、swap、cnot、cx、cy、cz、u3、u2、u1、crx、cry、crz、x1、y1、z1、cu1、cu3、u、cu、cr、iswap、id、ch、sqrtxdg、csqrtx、cswap、ph |
|2.1.2 |controls  |arrary-int  |n  |操作的控制比特 |
|2.1.3 |targets  |arrary-int  |n  |操作的目标比特 |
|2.1.4 |rotation  |arrary-double  |n  |旋转角度 |
|2.1.5 |desc  |string  |y  |量子操作描述 |
|2.1.6 |inverse  |int  |n  |是否执行逆操作，0：否，1：是 |
|2.1.7 |cmdex  |object  |n  |扩展命令 |
|2.1.7.1 |amp  |object  |n  |振幅数据 |
|2.1.7.1.1 |reals  |arrary-double  |y  |振幅实部 |
|2.1.7.1.2 |imags  |arrary-double  |y  |振幅虚部 |
|2.1.7.1.3 |startind  |int  |y  |振幅开始索引 |
|2.1.7.1.4 |numamps  |int  |y  |振幅结束索引 |
|2.1.7.2 |mat  |object  |n  |矩阵数据 |
|2.1.7.2.1 |reals  |arrary-double  |y  |矩阵实部 |
|2.1.7.2.2 |imags  |arrary-double  |y  |矩阵虚部 |
|2.1.7.2.3 |unitary  |bool  |y  |是否为酉矩阵 |
|3 |final  |bool  |n  |是否是最后的线路 |

响应说明：
| 序号 | 字段名称 | 类型 | 是否必填 | 说明 |
| ---- | ---- | ---- | ---- | ---- |
|1 |base  |object   |y  |返回码 |
|1.1 |code  |int   |y  |返回码，0：成功，其它值：参考ecode.thrift文件 |
|1.2 |msg  |string   |y  |返回信息描述 |

请求和响应数据（qusproutdata.thrift）定义：
```
//振幅数据
struct Amplitude {
    //振幅实部
    1: required list<double> reals

    //振幅虚部
    2: required list<double> imags

    //振幅开始索引
    3: required i32 startind

    //振幅结束索引
    4: required i32 numamps
}

//自定义矩阵
struct Matrix {
    //矩阵实部
    1: required list<list<double>> reals

    //矩阵虚部
    2: required list<list<double>> imags

    //是否为酉矩阵
    3: required bool unitary
}

//扩展指令
struct Cmdex {
    //振幅数据
    1: optional Amplitude amp
    //矩阵数据
    2: optional Matrix mat
}

//指令数据
struct Cmd {
    //指令门类型
    1: required string gate

    //目标集
    2: required list<i32> targets

    //控制集
    3: required list<i32> controls

    //旋转集
    4: required list<double> rotation

    //指令详情描述
    5: required string desc

    //是否执行逆操作
    6: required bool inverse

    //扩展命令
    7: optional Cmdex cmdex
}

//指令集
struct Circuit {
    //指令集
    1: required list<Cmd> cmds
}

//添加量子指令
struct SendCircuitCmdReq {
    //任务id
    1: required string id

    //指令集
    2: optional Circuit circuit

    //是否释放该请求资源
    3: optional bool final
}

struct SendCircuitCmdResp {
    //返回码
    1: required ecode.BaseCode base
}
```

接口文件（qusprout.thrift）定义：
```
service QuSproutServer {
    //添加量子指令
    qusproutdata.SendCircuitCmdResp sendCircuitCmd(1:qusproutdata.SendCircuitCmdReq req)

}
```

c++示例：
```
//连接QuSprout
std::shared_ptr<TSocket> socket = std::make_shared<TSocket>("127.0.0.1", 9091); 
std::shared_ptr<TTransport> transport = std::make_shared<TBufferedTransport>(socket);
std::shared_ptr<TProtocol> protocol = std::make_shared<TBinaryProtocol>(transport);
std::shared_ptr<TMultiplexedProtocol> quest = std::make_shared<TMultiplexedProtocol>(protocol, "QuSproutServer");
transport->open();
std::shared_ptr<QuSproutServerClient> client = std::make_shared<QuSproutServerClient>(quest);

std::ostringstream os("");
os << time(NULL);
auto id = os.str();

//初始化量子环境
......

//添加量子指令
SendCircuitCmdReq cmdreq;
cmdreq.__set_id(id);
Circuit circuit;
{
    Cmd cmd;
    cmd.__set_gate("x");
    cmd.targets.push_back(0);
    os.str("");
    os << "x q[0]";
    cmd.desc = os.str();
    cmd.inverse = false;
    circuit.cmds.push_back(cmd);
}
cmdreq.__set_circuit(circuit);
cmdreq.__set_final(false);
SendCircuitCmdResp cmdresp;
client->sendCircuitCmd(cmdresp, cmdreq);

//释放量子环境
......
```

### 2.3 重复线路操作
接口说明：
1.重复运行sendCircuitCmd添加的所有线路shots次；

2.改接口运行完成后，不能再做其它任何操作，运行完后线路会被释放；

请求说明：
| 序号 | 字段名称 | 类型 | 是否必填 | 说明 |
| ---- | ---- | ---- | ---- | ---- |
|1 |id  |string   |y  |任务id，同一任务保持一致 |
|2 |shots  |int  |y  |重复shots次运行整个线路 |

响应说明：
| 序号 | 字段名称 | 类型 | 是否必填 | 说明 |
| ---- | ---- | ---- | ---- | ---- |
|1 |base  |object   |y  |返回码 |
|1.1 |code  |int   |y  |返回码，0：成功，其它值：参考ecode.thrift文件 |
|1.2 |msg  |string   |y  |返回信息描述 |
|2 |result  |object   |n  |响应数据 |
|2.1 |measureSet  |arrary  |n  |数组,测量结果 |
|2.1.1 |id  |int  |y  |目标比特位 |
|2.1.2 |value  |int  |y  |测量的值（0或者1） |
|2.2 |outcomeSet |arrary  |n  |数组，目标出现概率 |
|2.2.1 |bitstr  |string  |y  |比特位组成的字符串，例如2个qubit时，那么有"00","01","10","11"的4个值 |
|2.2.2 |count  |int  |y  |目标出现的次数 |

请求和响应数据（qusproutdata.thrift）定义：
```
//测量结果
struct MeasureResult {
    //量子比特
    1: required i32 id

    //测量结果
    2: required i32 value
}

//运行结果
struct Outcome {
    //比特位组成的字符串
    1: required string bitstr

    //出现次数
    2: required i32 count
}

//指令结果集
struct Result {
    //测量结果
    1: required list<MeasureResult> measureSet

    //结果集
    2: required list<Outcome> outcomeSet
}

//执行任务
struct RunCircuitReq {
    //任务id
    1: required string id

    //运行次数
    2: required i32 shots
}

struct RunCircuitResp {
    //返回码
    1: required ecode.BaseCode base 

    //返回结果
    2: optional Result result
}
```

接口文件（qusprout.thrift）定义：
```
service QuSproutServer {
    //重复线路操作
    qusproutdata.RunCircuitResp run(1:qusproutdata.RunCircuitReq req)

}
```

c++示例：
```
//连接QuSprout
std::shared_ptr<TSocket> socket = std::make_shared<TSocket>("127.0.0.1", 9091); 
std::shared_ptr<TTransport> transport = std::make_shared<TBufferedTransport>(socket);
std::shared_ptr<TProtocol> protocol = std::make_shared<TBinaryProtocol>(transport);
std::shared_ptr<TMultiplexedProtocol> quest = std::make_shared<TMultiplexedProtocol>(protocol, "QuSproutServer");
transport->open();
std::shared_ptr<QuSproutServerClient> client = std::make_shared<QuSproutServerClient>(quest);

std::ostringstream os("");
os << time(NULL);
auto id = os.str();

//初始化量子环境
......

//添加量子指令
......

//重复线路操作
RunCircuitReq runreq;
runreq.__set_id(id);
runreq.__set_shots(100);
RunCircuitResp runresp;
client->run(runresp, runreq);
```

### 2.4 获取振幅
接口说明：
1.获取振幅；

请求说明：
| 序号 | 字段名称 | 类型 | 是否必填 | 说明 |
| ---- | ---- | ---- | ---- | ---- |
|1 |id  |string   |y  |任务id，同一任务保持一致 |
|2 |index  |int  |y  |目标索引，取值范围：[0, 2^qubitnum) |

响应说明：
| 序号 | 字段名称 | 类型 | 是否必填 | 说明 |
| ---- | ---- | ---- | ---- | ---- |
|1 |base  |object   |y  |返回码 |
|1.1 |code  |int   |y  |返回码，0：成功，其它值：参考ecode.thrift文件 |
|1.2 |msg  |string   |y  |返回信息描述 |
|2 |amp  |double  |y  |振幅 |

请求和响应数据（qusproutdata.thrift）定义：
```
//获取振幅
struct GetProbAmpReq {
    //任务id
    1: required string id

    //qubit索引
    2: required i64 index
}

struct GetProbAmpResp {
    //返回码
    1: required ecode.BaseCode base

    //振幅
    2: optional double amp
}
```

接口文件（qusprout.thrift）定义：
```
service QuSproutServer {
    //获取振幅
    qusproutdata.GetProbAmpResp getProbAmp(1:qusproutdata.GetProbAmpReq req)

}
```

c++示例：
```
//连接QuSprout
std::shared_ptr<TSocket> socket = std::make_shared<TSocket>("127.0.0.1", 9091); 
std::shared_ptr<TTransport> transport = std::make_shared<TBufferedTransport>(socket);
std::shared_ptr<TProtocol> protocol = std::make_shared<TBinaryProtocol>(transport);
std::shared_ptr<TMultiplexedProtocol> quest = std::make_shared<TMultiplexedProtocol>(protocol, "QuSproutServer");
transport->open();
std::shared_ptr<QuSproutServerClient> client = std::make_shared<QuSproutServerClient>(quest);

std::ostringstream os("");
os << time(NULL);
auto id = os.str();

//初始化量子环境
......

//添加量子指令
......

//获取振幅
GetProbAmpReq ampreq;
ampreq.__set_id(id);
ampreq.__set_index(0);
GetProbAmpResp ampresp;
client->getProbAmp(ampresp, ampreq);

//释放量子环境
......
```

### 2.5 获取概率
接口说明：
1.获取概率;

请求说明：
| 序号 | 字段名称 | 类型 | 是否必填 | 说明 |
| ---- | ---- | ---- | ---- | ---- |
|1 |id  |string   |y  |任务id，同一任务保持一致 |
|2 |qubit  |int  |y  |目标比特位，取值范围：[0, qubitnum) |
|3 |outcom  |int  |y  |需要获取0或者1的概率 |

响应说明：
| 序号 | 字段名称 | 类型 | 是否必填 | 说明 |
| ---- | ---- | ---- | ---- | ---- |
|1 |base  |object   |y  |返回码 |
|1.1 |code  |int   |y  |返回码，0：成功，其它值：参考ecode.thrift文件 |
|1.2 |msg  |string   |y  |返回信息描述 |
|2 |pro_outcome  |double  |y  |目标比特为0或者1的概率 |

请求和响应数据（qusproutdata.thrift）定义：
```
//获取当前qubit的概率
struct GetProbOfOutcomeReq {
    //任务id
    1: required string id

    //qubit索引
    2: required i32 qubit

    //需要获取0或者1的概率
    3: required i32 outcom
}

struct GetProbOfOutcomeResp {
    //返回码
    1: required ecode.BaseCode base

    //概率
    2: optional double pro_outcome
}
```

接口文件（qusprout.thrift）定义：
```
service QuSproutServer {
    //获取当前qubit的概率
    qusproutdata.GetProbOfOutcomeResp getProbOfOutcome(1:qusproutdata.GetProbOfOutcomeReq req)

}
```

c++示例：
```
//连接QuSprout
std::shared_ptr<TSocket> socket = std::make_shared<TSocket>("127.0.0.1", 9091); 
std::shared_ptr<TTransport> transport = std::make_shared<TBufferedTransport>(socket);
std::shared_ptr<TProtocol> protocol = std::make_shared<TBinaryProtocol>(transport);
std::shared_ptr<TMultiplexedProtocol> quest = std::make_shared<TMultiplexedProtocol>(protocol, "QuSproutServer");
transport->open();
std::shared_ptr<QuSproutServerClient> client = std::make_shared<QuSproutServerClient>(quest);

std::ostringstream os("");
os << time(NULL);
auto id = os.str();

//初始化量子环境
......

//添加量子指令
......

//获取振幅
GetProbOfOutcomeReq alloutcomreq;
alloutcomreq.__set_id(id);
alloutcomreq.__set_qubit(0);
alloutcomreq.__set_outcom(0);
GetProbOfOutcomeResp alloutcomresp;
client->getProbOfOutcome(alloutcomresp, alloutcomreq);

//释放量子环境
......
```

### 2.6 获取组合概率
接口说明：
1.获取组合概率;

请求说明：
| 序号 | 字段名称 | 类型 | 是否必填 | 说明 |
| ---- | ---- | ---- | ---- | ---- |
|1 |id  |string   |y  |任务id，同一任务保持一致 |
|2 |targets  |int  |y  |目标比特列表，取值范围：[0, qubitnum) |

响应说明：
| 序号 | 字段名称 | 类型 | 是否必填 | 说明 |
| ---- | ---- | ---- | ---- | ---- |
|1 |base  |object   |y  |返回码 |
|1.1 |code  |int   |y  |返回码，0：成功，其它值：参考ecode.thrift文件 |
|1.2 |msg  |string   |y  |返回信息描述 |
|2 |pro_outcomes  |arrary-double  |y  |目标比特的组合概率 |

请求和响应数据（qusproutdata.thrift）定义：
```
//获取组合概率
struct GetProbOfAllOutcomReq {
    //任务id
    1: required string id

    //概率目标列表
    2: required list<i32> targets
}

struct GetProbOfAllOutcomResp {
    //返回码
    1: required ecode.BaseCode base

    //概率
    2: optional list<double> pro_outcomes
}
```

接口文件（qusprout.thrift）定义：
```
service QuSproutServer {
    //获取组合概率
    qusproutdata.GetProbOfAllOutcomResp getProbOfAllOutcome(1:qusproutdata.GetProbOfAllOutcomReq req)

}
```

c++示例：
```
//连接QuSprout
std::shared_ptr<TSocket> socket = std::make_shared<TSocket>("127.0.0.1", 9091); 
std::shared_ptr<TTransport> transport = std::make_shared<TBufferedTransport>(socket);
std::shared_ptr<TProtocol> protocol = std::make_shared<TBinaryProtocol>(transport);
std::shared_ptr<TMultiplexedProtocol> quest = std::make_shared<TMultiplexedProtocol>(protocol, "QuSproutServer");
transport->open();
std::shared_ptr<QuSproutServerClient> client = std::make_shared<QuSproutServerClient>(quest);

std::ostringstream os("");
os << time(NULL);
auto id = os.str();

//初始化量子环境
......

//添加量子指令
......

//获取组合概率
GetProbOfAllOutcomReq alloutcomreq;
alloutcomreq.__set_id(id);
std::vector<int32_t> targets;
for (auto i = 0; i < qubitnum; ++i)
{
    targets.push_back(i);
}
alloutcomreq.__set_targets(targets);
GetProbOfAllOutcomResp alloutcomresp;
client->getProbOfAllOutcome(alloutcomresp, alloutcomreq);

//释放量子环境
......
```

### 2.7 获取量子执行状态
接口说明：
1.获取量子执行状态;

请求说明：
| 序号 | 字段名称 | 类型 | 是否必填 | 说明 |
| ---- | ---- | ---- | ---- | ---- |
|1 |id  |string   |y  |任务id，同一任务保持一致 |

响应说明：
| 序号 | 字段名称 | 类型 | 是否必填 | 说明 |
| ---- | ---- | ---- | ---- | ---- |
|1 |base  |object   |y  |返回码 |
|1.1 |code  |int   |y  |返回码，0：成功，其它值：参考ecode.thrift文件 |
|1.2 |msg  |string   |y  |返回信息描述 |
|2 |all_state  |arrary-string  |y  |每个状态位的实部和虚部的组成的字符串，格式位：real,imag，数量为2^qubitnum |

请求和响应数据（qusproutdata.thrift）定义：
```
//获取所有的计算状态
struct GetAllStateReq {
    //任务id
    1: required string id
}

struct GetAllStateResp {
    //返回码
    1: required ecode.BaseCode base

    //状态
    2: optional list<string> all_state
}
```

接口文件（qusprout.thrift）定义：
```
service QuSproutServer {
    //获取所有的计算结果
    qusproutdata.GetAllStateResp getAllState(1:qusproutdata.GetAllStateReq req)

}
```

c++示例：
```
//连接QuSprout
std::shared_ptr<TSocket> socket = std::make_shared<TSocket>("127.0.0.1", 9091); 
std::shared_ptr<TTransport> transport = std::make_shared<TBufferedTransport>(socket);
std::shared_ptr<TProtocol> protocol = std::make_shared<TBinaryProtocol>(transport);
std::shared_ptr<TMultiplexedProtocol> quest = std::make_shared<TMultiplexedProtocol>(protocol, "QuSproutServer");
transport->open();
std::shared_ptr<QuSproutServerClient> client = std::make_shared<QuSproutServerClient>(quest);

std::ostringstream os("");
os << time(NULL);
auto id = os.str();

//初始化量子环境
......

//添加量子指令
......

//获取所有的计算结果
GetAllStateReq statereq;
statereq.__set_id(id);
GetAllStateResp stateresp;
client->getAllState(stateresp, statereq);

//释放量子环境
......
```

### 2.8 释放量子环境
接口说明：
1.释放量子环境;

请求说明：
| 序号 | 字段名称 | 类型 | 是否必填 | 说明 |
| ---- | ---- | ---- | ---- | ---- |
|1 |id  |string   |y  |任务id，同一任务保持一致 |

响应说明：
| 序号 | 字段名称 | 类型 | 是否必填 | 说明 |
| ---- | ---- | ---- | ---- | ---- |
|1 |base  |object   |y  |返回码 |
|1.1 |code  |int   |y  |返回码，0：成功，其它值：参考ecode.thrift文件 |
|1.2 |msg  |string   |y  |返回信息描述 |

请求和响应数据（qusproutdata.thrift）定义：
```
//释放量子环境
struct CancelCmdReq {
    //任务id
    1: required string id
}

struct CancelCmdResp {
    //返回码
    1: required ecode.BaseCode base
}
```

接口文件（qusprout.thrift）定义：
```
service QuSproutServer {
    //释放量子环境
    qusproutdata.CancelCmdResp cancelCmd(1:qusproutdata.CancelCmdReq req)

}
```

c++示例：
```
//连接QuSprout
std::shared_ptr<TSocket> socket = std::make_shared<TSocket>("127.0.0.1", 9091); 
std::shared_ptr<TTransport> transport = std::make_shared<TBufferedTransport>(socket);
std::shared_ptr<TProtocol> protocol = std::make_shared<TBinaryProtocol>(transport);
std::shared_ptr<TMultiplexedProtocol> quest = std::make_shared<TMultiplexedProtocol>(protocol, "QuSproutServer");
transport->open();
std::shared_ptr<QuSproutServerClient> client = std::make_shared<QuSproutServerClient>(quest);

std::ostringstream os("");
os << time(NULL);
auto id = os.str();

//初始化量子环境
......

//释放量子环境
CancelCmdReq cancelreq;
cancelreq.__set_id(id);
CancelCmdResp cancelresp;
client->cancelCmd(cancelresp, cancelreq);
```

### 2.9 获取泡利算子乘积的期望值
接口说明：
1.获取泡利算子乘积的期望值;

请求说明：
| 序号 | 字段名称 | 类型 | 是否必填 | 说明 |
| ---- | ---- | ---- | ---- | ---- |
|1 |id  |string   |y  |任务id，同一任务保持一致 |
|2 |pauli_prod  |object   |y  |期望值信息 |
|2.1 |oper_type  |enum   |y  |泡利算子操作类型 0:I 1:x 2:Y 3:Z |
|2.2 |target  |iny   |y  |目标比特位，取值范围：[0, qubitnum) |

响应说明：
| 序号 | 字段名称 | 类型 | 是否必填 | 说明 |
| ---- | ---- | ---- | ---- | ---- |
|1 |base  |object   |y  |返回码 |
|1.1 |code  |int   |y  |返回码，0：成功，其它值：参考ecode.thrift文件 |
|1.2 |msg  |string   |y  |返回信息描述 |
|2 |expect  |double   |y  |期望值 |

请求和响应数据（qusproutdata.thrift）定义：
```
//泡利算子操作类型
enum PauliOperType {
    POT_PAULI_I = 0, 
    POT_PAULI_X = 1, 
    POT_PAULI_Y = 2, 
    POT_PAULI_Z = 3
}

struct PauliProdInfo {
    //泡利算子操作类型
    1: required PauliOperType oper_type

    //目标比特位
    2: required i32 target
}

//获取泡利算子乘积的期望值
struct GetExpecPauliProdReq {
    //任务id
    1: required string id

    //期望值信息
    2: required list<PauliProdInfo> pauli_prod
}

struct GetExpecPauliProdResp {
    //返回码
    1: required ecode.BaseCode base 

    //期望值
    2: optional double expect
}
```

接口文件（qusprout.thrift）定义：
```
service QuSproutServer {
    //获取泡利算子乘积的期望值
    qusproutdata.GetExpecPauliProdResp getExpecPauliProd(1:qusproutdata.GetExpecPauliProdReq req)

}
```

c++示例：
```
//连接QuSprout
std::shared_ptr<TSocket> socket = std::make_shared<TSocket>("127.0.0.1", 9091); 
std::shared_ptr<TTransport> transport = std::make_shared<TBufferedTransport>(socket);
std::shared_ptr<TProtocol> protocol = std::make_shared<TBinaryProtocol>(transport);
std::shared_ptr<TMultiplexedProtocol> quest = std::make_shared<TMultiplexedProtocol>(protocol, "QuSproutServer");
transport->open();
std::shared_ptr<QuSproutServerClient> client = std::make_shared<QuSproutServerClient>(quest);

std::ostringstream os("");
os << time(NULL);
auto id = os.str();

const int qubitnum = 3;
//初始化量子环境
......

//获取泡利算子乘积的期望值
GetExpecPauliProdReq pauliprodreq;
pauliprodreq.__set_id(id);
std::vector<PauliProdInfo> pauliprod;
for (auto i = 0; i < qubitnum; ++i)
{
    PauliProdInfo temp;
    temp.__set_target(i);
    temp.__set_oper_type((PauliOperType::type)(rand() % 4));
    pauliprod.push_back(temp);
}
pauliprodreq.__set_pauli_prod(pauliprod);
GetExpecPauliProdResp pauliprodresp;
client->getExpecPauliProd(pauliprodresp, pauliprodreq);

//释放量子环境
......
```

### 2.10 获取泡利算子乘积之和的期望值
接口说明：
1.获取泡利算子乘积之和的期望值;

请求说明：
| 序号 | 字段名称 | 类型 | 是否必填 | 说明 |
| ---- | ---- | ---- | ---- | ---- |
|1 |id  |string   |y  |任务id，同一任务保持一致 |
|2.1 |oper_type_list  |enum   |y  |泡利算子操作类型，注意：oper_type_list的数量和qubitnum*term_coeff_list的数量相等 0:I 1:x 2:Y 3:Z |
|2.2 |term_coeff_list  |int   |y  |回归系数 |

响应说明：
| 序号 | 字段名称 | 类型 | 是否必填 | 说明 |
| ---- | ---- | ---- | ---- | ---- |
|1 |base  |object   |y  |返回码 |
|1.1 |code  |int   |y  |返回码，0：成功，其它值：参考ecode.thrift文件 |
|1.2 |msg  |string   |y  |返回信息描述 |
|2 |expect  |double   |y  |期望值 |

请求和响应数据（qusproutdata.thrift）定义：
```
//泡利算子操作类型
enum PauliOperType {
    POT_PAULI_I = 0, 
    POT_PAULI_X = 1, 
    POT_PAULI_Y = 2, 
    POT_PAULI_Z = 3
}

//获取泡利算子乘积之和的期望值
struct GetExpecPauliSumReq {
    //任务id
    1: required string id

    //泡利算子操作类型，注意：oper_type_list的数量和qubitnum*term_coeff_list的数量相等
    2: required list<PauliOperType> oper_type_list

    //回归系数
    3: required list<double> term_coeff_list
}

struct GetExpecPauliSumResp {
    //返回码
    1: required ecode.BaseCode base 

    //期望值
    2: optional double expect
}
```

接口文件（qusprout.thrift）定义：
```
service QuSproutServer {
    //获取泡利算子乘积之和的期望值
    qusproutdata.GetExpecPauliSumResp getExpecPauliSum(1:qusproutdata.GetExpecPauliSumReq req)

}
```

c++示例：
```
//连接QuSprout
std::shared_ptr<TSocket> socket = std::make_shared<TSocket>("127.0.0.1", 9091); 
std::shared_ptr<TTransport> transport = std::make_shared<TBufferedTransport>(socket);
std::shared_ptr<TProtocol> protocol = std::make_shared<TBinaryProtocol>(transport);
std::shared_ptr<TMultiplexedProtocol> quest = std::make_shared<TMultiplexedProtocol>(protocol, "QuSproutServer");
transport->open();
std::shared_ptr<QuSproutServerClient> client = std::make_shared<QuSproutServerClient>(quest);

std::ostringstream os("");
os << time(NULL);
auto id = os.str();

const int qubitnum = 3;
//初始化量子环境
......

//获取泡利算子乘积之和的期望值
int termsnum = 1;
time_t now = time(NULL);
std::vector<PauliOperType::type> oper_type_list;
std::vector<double> term_coeff_list;
GetExpecPauliSumReq paulisumreq;
paulisumreq.__set_id(id);
for (auto i = 0; i < termsnum; ++i)
{
    srand(now + i);
    double coeff = rand() % 1000;
    coeff += coeff/100.0;
    if (rand() % 2 == 0)
    {
        coeff = -coeff;
    }
    term_coeff_list.push_back(coeff);
}
for (auto i = 0; i < termsnum*qubitnum; ++i)
{
    srand(now + i);
    oper_type_list.push_back((PauliOperType::type)(rand() % 4));
}
paulisumreq.__set_oper_type_list(oper_type_list);
paulisumreq.__set_term_coeff_list(term_coeff_list);
GetExpecPauliSumResp paulisumresp;
client->getExpecPauliSum(paulisumresp, paulisumreq);

//释放量子环境
......
```

### 2.11 获取测量结果
接口说明：
1.获取测量结果；

请求说明：
| 序号 | 字段名称 | 类型 | 是否必填 | 说明 |
| ---- | ---- | ---- | ---- | ---- |
|1 |id  |string   |y  |任务id，同一任务保持一致 |
|2 |qubits  |arrary-int  |y  |指定要获取的测量的量子比特位，取值范围：[0, qubitnum) |

响应说明：
| 序号 | 字段名称 | 类型 | 是否必填 | 说明 |
| ---- | ---- | ---- | ---- | ---- |
|1 |base  |object   |y  |返回码 |
|1.1 |code  |int   |y  |返回码，0：成功，其它值：参考ecode.thrift文件 |
|1.2 |msg  |string   |y  |返回信息描述 |
|2 |measureSet  |arrary  |n  |数组,测量结果 |
|2.1 |id  |int  |y  |目标比特位 |
|2.2 |value  |int  |y  |测量的值（0或者1） |
|3 |outcomeSet |arrary  |n  |数组，目标出现概率 |
|3.1 |bitstr  |string  |y  |比特位组成的字符串，例如2个qubit时，那么有"00","01","10","11"的4个值 |
|3.2 |count  |int  |y  |目标出现的次数 |

请求和响应数据（qusproutdata.thrift）定义：
```
//测量结果
struct MeasureResult {
    //量子比特
    1: required i32 id

    //测量结果
    2: required i32 value
}

//运行结果
struct Outcome {
    //比特位组成的字符串
    1: required string bitstr

    //出现次数
    2: required i32 count
}

//获取测量结果
struct MeasureQubitsReq {
    // 任务id。
    1: required string id

    // 指定要 measure 的 qubits 。
    2: required list<i32> qubits
}

struct MeasureQubitsResp {
    //返回码
    1: required ecode.BaseCode base

    //测量结果
    2: optional list<MeasureResult> results

    //结果集
    3: required list<Outcome> outcomes
}
```

接口文件（qusprout.thrift）定义：
```
service QuSproutServer {
    //获取测量结果
    qusproutdata.MeasureQubitsResp measureQubits(1:qusproutdata.MeasureQubitsReq req)

}
```

c++示例：
```
//连接QuSprout
std::shared_ptr<TSocket> socket = std::make_shared<TSocket>("127.0.0.1", 9091); 
std::shared_ptr<TTransport> transport = std::make_shared<TBufferedTransport>(socket);
std::shared_ptr<TProtocol> protocol = std::make_shared<TBinaryProtocol>(transport);
std::shared_ptr<TMultiplexedProtocol> quest = std::make_shared<TMultiplexedProtocol>(protocol, "QuSproutServer");
transport->open();
std::shared_ptr<QuSproutServerClient> client = std::make_shared<QuSproutServerClient>(quest);

std::ostringstream os("");
os << time(NULL);
auto id = os.str();

//初始化量子环境
......

const int qubitnum = 3;
//添加量子指令
......

//获取测量结果
MeasureQubitsReq measurereq;
measurereq.__set_id(id);
std::vector<int32_t> measurequbits;
for (auto i = 0; i < qubitnum; ++i)
{
    measurequbits.push_back(i);
}
measurereq.__set_qubits(measurequbits);
MeasureQubitsResp measureresp;
client->measureQubits(measureresp, measurereq);

//释放量子环境
......
```

## 3 错误码定义

| 序号 | 返回码 | 返回码描述 |
| ---- | ---- | ---- |
|1 |0  |成功   |
|2 |1  |未知错误   |
|3 |2  |不合法参数   |
|4 |3  |超时   |
|5 |4  |异常  |
|6 |5  |不存在  |
|7 |6 |存在  |
|8 |7  |缓存中  |
|9 |8  |内存不足  |
|10 |9  |未初始化  |
|11 |10  |机器未注册  |
|12 |11  |无可用的资源  |
|13 |12  |数据解析异常  |
