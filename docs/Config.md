# QuSprout配置文件文档
## 1 概述
文档对组成QuSprout的master，slave，work服务的配置文件进行了详细的说明。

## 2 配置文件说明
### 2.1 master配置文件
配置文件名：
qusprout-master.yaml

默认安装路径：
/usr/local/etc/qusprout

配置参数说明：
| 序号 | 配置项 | 值类型  | 默认值 | 说明 |
| ---- | ---- | ---- | ---- | ---- |
|1  |log  |object  |-  |日志配置  |
|1.1  |logLevel  |int  |0  |日志级别 0：info 1：warning 2：error 3：fatal  |
|1.2  |logPath  |string  |/var/log  |日志的路径  |
|1.3  |logNamePrefix  |string  |master  |日志文件名前缀  |
|2  |rpc  |object  |-  |rpc配置  |
|2.1  |listenPort  |int  |9091  |rpc对外的api监听的端口  |
|2.2  |sendTimeout  |int  |0  |rpc对外的api发送数据超时时间，单位：秒,0代表不设置超时  |
|2.3  |recvTimeout  |int  |0  |rpc对外的api接收数据超时时间，单位：秒,0代表不设置超时  |
|2.4  |listenSysPort  |int  |9092  |提交slave注册、上报数据等操作的端口  |
|3  |task  |object  |-  |任务配置  |
|3.1  |cleanTaskInterval  |int  |150  |任务定时清理的时间间隔，单位：秒  |
|3.2  |cleanTaskTimeout  |arrary  |-  |任务各个状态的超时时间  |
|3.2.*.1  |taskState  |int  |-  |任务的状态，0：initial, 1：initialized, 2:queued, 3:running, 4:done, 5:error  |
|3.2.*.2  |timeout  |int  |-  |任务各个状态的超时时间，单位：秒  |
|4  |resource  |object  |-  |资源配置  |
|4.1  |resourceTimeOutDuration  |int  |150  |slave资源超时时间，单位：秒  |
|4.2  |resourceHeartInterval  |int  |20  |slave资源定时清理的时间间隔，单位：秒  |
|5  |client  |object  |-  |资源配置  |
|5.1  |clientConnTimeout  |int  |300000  |连接资源超时时间，单位：毫秒  |
|5.2  |clientSendTimeout  |int  |300000  |发送数据给资源超时时间，单位：毫秒  |
|5.3  |clientRecvTimeout  |int  |300000  |接收资源的数据超时时间，单位：毫秒  |
|6  |quroot  |object  |-  |QuRoot配置  |
|6.1  |enable  |int  |0  |是否向QuRoot注册的开关，0：关 1：开  |
|6.2  |port  |int  |8002  |QuRoot的端口  |
|6.3  |addr  |string  |127.0.0.1  |QuRoot监听的ip  |
|6.4  |qurootHeartInterval  |int  |3  |和QuRoot的心跳间隔  |
|7  |cap  |object  |-  |QuSprout能力  |
|7.1  |deviceType  |string  |cpu  |支持的设备类型  |
|7.2  |supGate  |string  |h,measure,p,cp,r,rx,ry,rz,rxx,ryy,rzz,x,y,z,s,t,sdg,tdg,sqrtx,sqrtswap,swap,cnot,cx,cy,cz,u3,u2,u1,crx,cry,crz,x1,y1,z1,cu1,cu3,u,cu,cr,iswap,id,ch,sqrtxdg,csqrtx,cswap,ph  |支持的量子门操作  |
|8  |http  |object  |-  |QuSprout的http的api  |
|8.1  |listenHttpPort  |int  |9093  |http监听的端口  |
|8.2  |httpThreadNum  |int  |5  |处理http接口的线程数  |

### 2.2 slaver配置文件
配置文件名：
qusprout-slave.yaml

默认安装路径：
/usr/local/etc/qusprout

配置参数说明：
| 序号 | 配置项 | 值类型  | 默认值 | 说明 |
| ---- | ---- | ---- | ---- | ---- |
|1  |log  |object  |-  |日志配置  |
|1.1  |logLevel  |int  |0  |日志级别 0：info 1：warning 2：error 3：fatal  |
|1.2  |logPath  |string  |/var/log  |日志的路径  |
|1.3  |logNamePrefix  |string  |slave  |日志文件名前缀  |
|2  |common  |object  |-  |公共配置  |
|1.3  |verbose  |int  |0  |是否打印调试日志  |
|1.3  |processName  |string  |qusprout-slave  |日志文件名  |
|3  |rpc  |object  |-  |rpc配置  |
|3.1  |listenPort  |int  |9095  |rpc监听的端口  |
|3.2  |sendTimeout  |int  |0  |rpc发送数据超时时间，单位：秒,0代表不设置超时  |
|3.3  |recvTimeout  |int  |0  |rpc接收数据超时时间，单位：秒,0代表不设置超时  |
|4  |task  |object  |-  |任务配置  |
|4.1  |cleanTaskInterval  |int  |150  |任务定时清理的时间间隔，单位：秒  |
|4.2  |taskTimeOutDuration  |int  |300  |任务的超时时间，单位：秒  |
|5  |register  |object  |-  |注册的配置  |
|5.1  |masterHeartInterval  |int  |20  |和master服务的心跳间隔，单位：秒  |
|6  |master  |object  |-  |master服务配置  |
|6.1  |masterPort  |int  |9092  |master的rpc监听端口  |
|6.2  |masterAddr  |string  |127.0.0.1  |master的rpc监听ip  |
|7  |work  |object  |-  |work进程配置  |
|7.1  |workBinPath  |string  |/usr/local/bin |work进程的二进制文件路径  |
|7.2  |workBinName  |string  |qusprout-work  |work进程的二进制文件名  |
|7.3  |workConfigPath  |string  |/usr/local/etc/qusprout  |work进程的执行配置文件路径  |
|7.4  |workConfigName  |string  |qusprout-work.yaml  |work进程的执行配置文件名  |
|7.5  |waitRpcTimeout  |int  |120  |等待work以非MPI方式执行操作的超时时间  |
|7.6  |waitMpiRpcTimeout  |int  |300  |等待work以MPI方式执行操作的超时时间  |
|8  |client  |object  |-  |资源配置  |
|8.1  |clientConnTimeout  |int  |300000  |连接资源超时时间，单位：毫秒  |
|8.2  |clientSendTimeout  |int  |300000  |发送数据给资源超时时间，单位：毫秒  |
|8.3  |clientRecvTimeout  |int  |300000  |接收资源的数据超时时间，单位：毫秒  |


### 2.3 work配置文件
配置文件名：
qusprout-work.yaml

默认安装路径：
/usr/local/etc/qusprout

配置参数说明：
| 序号 | 配置项 | 值类型  | 默认值 | 说明 |
| ---- | ---- | ---- | ---- | ---- |
|1  |log  |object  |-  |日志配置  |
|1.1  |loglevel  |int  |0  |日志级别 0：info 1：warning 2：error 3：fatal  |
|1.2  |logpath  |string  |/var/log  |日志的路径  |
|1.3  |appname  |string  |qusprout-work  |日志文件名  |
|2  |common  |object  |-  |公共配置  |
|1.3  |verbose  |int  |0  |是否打印调试日志  |