# QuSprout说明文档
## 1 概述

## 2 QuSprout安装
### 2.1 QuSprout的裸机安装
#### 2.1.1 环境安装
```
cd ./thirdparty
chmod +x install.sh
sudo ./install.sh
```

#### 2.1.2 编译
```
```

#### 2.1.3 运行
```
```

### 2.2 QuSprout的docker安装
#### 2.2.1 安装docker
```
curl -fsSL https://get.docker.com | bash -s docker --mirror Aliyun
```

#### 2.2.2 编译镜像编译
```
```

#### 2.2.3 服务镜像编译
```
```

#### 2.2.4 运行镜像
```
```

## 3 监控的安装和配置
### 3.1 下载监控服务镜像
```
docker pull prom/prometheus
docker pull grafana/grafana
docker pull prom/node-exporter
```

### 3.2 运行镜像
1、运行prometheus
```
# vim prometheus.yml
global:
  scrape_interval: 15s
  evaluation_interval: 15s

alerting:
  alertmanagers:
    - static_configs:
        - targets:
          # - alertmanager:9093

rule_files:
  # - "first_rules.yml"
  # - "second_rules.yml"

scrape_configs:
  - job_name: "prometheus"
    static_configs:
      - targets: ["localhost:9090"]
  - job_name: linux
    static_configs:
      - targets: ['${qusprout_ip}:9100']
  - job_name: qusprout
    static_configs:
      - targets: ['${qusprout_ip}:9093']
```
```  
docker run -itd --name=docker_prometheus --restart=always -p 9090:9090 -v /root/prometheus/prometheus.yml:/etc/prometheus/prometheus.yml -v "/etc/localtime:/etc/localtime" prom/prometheus
web地址：http://${qusprout_ip}:9090
```

2、运行grafana
```
mkdir -p /root/grafana
chmod 777 /root/grafana
chmod 777 /root/grafana -R
docker run -itd --name=grafana --restart=always -p 3000:3000 -v /root/grafana:/var/lib/grafana -v "/etc/localtime:/etc/localtime" grafana/grafana
web地址：http://${qusprout_ip}:3000
登录账号：admin/admin
```

3、运行node-exporter
```
docker run -itd --name=node-exporter --restart=always -p 9100:9100 -v "/etc/localtime:/etc/localtime" prom/node-exporter
web地址：http://${qusprout_ip}:9100/metrics
```

4、查看运行状态
```
# docker ps -a
CONTAINER ID   IMAGE                COMMAND                  CREATED          STATUS          PORTS                                       NAMES
017c5cd71058   prom/node-exporter   "/bin/node_exporter"     5 seconds ago    Up 4 seconds    0.0.0.0:9100->9100/tcp, :::9100->9100/tcp   node-exporter
5c09bbe84816   grafana/grafana      "/run.sh"                4 minutes ago    Up 4 minutes    0.0.0.0:3000->3000/tcp, :::3000->3000/tcp   grafana
9da6ed484f1a   prom/prometheus      "/bin/prometheus --c…"   12 minutes ago   Up 12 minutes   0.0.0.0:9090->9090/tcp, :::9090->9090/tcp   docker_prometheus
```

### 3.3 配置监控
1、登录http://${qusprout_ip}:3000，在首页，选择 "Add your first data source"；

2、选择 Prometheus，填写http-->url-->http://${qusprout_ip}:9090，在选择save/test；

3、选择导入面板，选择8919；

4、选择导入面板，选择doc/监控面板/QuSprout-*.json；


## 4 接口
1、QuSprout采用Apache thrift定义接口语言定义接口文件，然后根据thrift命令生成*.cpp和*.h接口文件；

### 4.1 初始化量子环境
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

### 4.2 添加量子指令
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

### 4.11 错误码定义

| 序号 | 返回码 | 返回码描述 |
| ---- | ---- | ---- |
