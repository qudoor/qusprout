# QuSprout配置文件文档
## 1 概述
文档对组成QuSprout的qusprout，quwork服务的配置文件进行了详细的说明。

## 2 配置文件说明
### 2.1 qusprout服务配置文件
配置文件名：
qusprout.yaml

默认安装路径：
/usr/local/etc/qusprout

配置参数说明：
| <div style="width:40px">序号</div> | <div style="width:200px">配置项</div> | <div style="width:60px">值类型</div>  | <div style="width:400px">默认值</div> | <div style="width:300px">说明</div> |
| ---- | ---- | ---- | ---- | ---- |
|1  |log  |object  |-  |日志配置  |
|1.1  |logLevel  |int  |0  |日志级别 0：info 1：warning 2：error 3：fatal  |
|1.2  |logPath  |string  |/var/log  |日志的路径  |
|1.3  |logNamePrefix  |string  |qusprout  |日志文件名前缀  |
|2  |rpc  |object  |-  |rpc配置  |
|2.1  |listenPort  |int  |9091  |rpc对外的api监听的端口  |
|2.2  |sendTimeout  |int  |0  |rpc对外的api发送数据超时时间，单位：秒,0代表不设置超时  |
|2.3  |recvTimeout  |int  |0  |rpc对外的api接收数据超时时间，单位：秒,0代表不设置超时  |
|3  |task  |object  |-  |任务配置  |
|3.1  |cleanTaskInterval  |int  |150  |任务定时清理的时间间隔，单位：秒  |
|3.2  |cleanTaskTimeout  |arrary  |-  |任务各个状态的超时时间  |
|3.2.1  |taskState  |int  |-  |任务的状态，0：initial, 1：initialized, 2:queued, 3:running, 4:done, 5:error  |
|3.2.2  |timeout  |int  |-  |任务各个状态的超时时间，单位：秒  |
|4  |client  |object  |-  |资源配置  |
|4.1  |clientConnTimeout  |int  |300000  |连接资源超时时间，单位：毫秒  |
|4.2  |clientSendTimeout  |int  |300000  |发送数据给资源超时时间，单位：毫秒  |
|4.3  |clientRecvTimeout  |int  |300000  |接收资源的数据超时时间，单位：毫秒  |
|5  |quroot  |object  |-  |QuRoot配置  |
|5.1  |enable  |int  |0  |是否向QuRoot注册的开关，0：关 1：开  |
|5.2  |port  |int  |8002  |QuRoot的端口  |
|5.3  |addr  |string  |127.0.0.1  |QuRoot监听的ip  |
|5.4  |qurootHeartInterval  |int  |3  |和QuRoot的心跳间隔  |
|6  |cap  |object  |-  |QuSprout能力  |
|6.1  |deviceType  |string  |cpu  |支持的设备类型  |
|6.2  |supGate  |string  |h,measure,p,cp,r,rx,ry,rz,rxx,ryy,rzz,x,y,z,s,t,sdg,tdg,sqrtx,<br>sqrtswap,swap,cnot,cx,cy,cz,u3,u2,u1,crx,cry,crz,x1,y1,z1,<br>cu1,cu3,u,cu,cr,iswap,id,ch,sqrtxdg,csqrtx,cswap,ph  |支持的量子门操作  |
|7  |http  |object  |-  |QuSprout的http的api  |
|7.1  |listenHttpPort  |int  |9093  |http监听的端口  |
|7.2  |httpThreadNum  |int  |5  |处理http接口的线程数  |

### 2.2 quwork服务配置文件
配置文件名：
quwork.yaml

默认安装路径：
/usr/local/etc/qusprout

配置参数说明：
| <div style="width:40px">序号</div> | <div style="width:200px">配置项</div> | <div style="width:60px">值类型</div>  | <div style="width:500px">默认值</div> | <div style="width:300px">说明</div> |
| ---- | ---- | ---- | ---- | ---- |
|1  |log  |object  |-  |日志配置  |
|1.1  |loglevel  |int  |0  |日志级别 0：info 1：warning 2：error 3：fatal  |
|1.2  |logpath  |string  |/var/log  |日志的路径  |
|1.3  |appname  |string  |quwork  |日志文件名  |
|2  |common  |object  |-  |公共配置  |
|1.3  |verbose  |int  |0  |是否打印调试日志  |