## QuSprout

[![Documentation Status](https://img.shields.io/badge/docs-latest-brightgreen.svg)](http://developer.queco.cn/qutrunk_api/)
[![License](https://img.shields.io/badge/license-Apache%202-blue.svg)](LICENSE)
[![Download Code](https://img.shields.io/badge/download-zip-green.svg)](https://github.com/queco-quantum/qutrunk/archive/refs/heads/main.zip)


### **概述**
---
* QuSprout 是启科量子自主研发的一款免费、开源的量子计算模拟后端，用户在 QuTrunk 量子编程框架生成量子线路后, 如果需要更高的运行效率，需要连接到 QuSprout 计算后端来进行模拟计算。
* QuSprout 支持多个量子线路的任务管理、资源管理和并行计算。
* QuSprout 使用 C++ 作为宿主语言，支持在 Linux, MacOS X 上编译部署。
* QuSprout 提供了 Master，Slave，Work 服务模块来支持量子计算的模拟：
    * Master 服务通过 RPC 与 QuTrunk 量子编程框架连接，主要负责量子线路的任务管理、Slave 服务器的资源管理。
    * Slave 服务通过 RPC 与 Master服务连接，主要负责向 Master 服务注册服务器资源、量子线路任务管理、启动 Work 服务。
    * Work 服务主要负责量子线路的计算并返回计算结果。


### **服务模块**
---
##### Master Service:

  1. 启动 Master 服务后，Master 会等待接收 Slave 服务发送的注册信息，包括 Slave 服务器的主机信息（主机名，版本等）和资源（内存，GPU 等）信息，Master 会维护所有注册的 Slave 服务器资源信息。
  2. 当 Master 服务接收到来自 QuTrunk 量子编程框架的量子线路的初始化请求时，会先检查是否有足够的资源来运行该量子线路（通过计算 qubits 所需的内存）。
  3. 如果资源要求满足，Master 会按照一定的算法从维护的slave服务器资源列表中选取一个slave服务器来运行该量子算法，并与该slave服务器建立 RPC 连接后向该服务器发送量子线路初始化请求，然后将量子线路加入到任务列表，Master 会维护一个任务列表管理所有的量子线路请求。
  4. Master 会定时清理一些长时间没有更新的任务，具体超时配置在master配置文件中，更多配置相关的内容请参考[配置文件](./Config.md)

##### Slave Service:

  1. 启动 Slave 服务后，Slave 会向 Master 服务注册信息，包括主机信息（主机名，版本等）和资源（内存，GPU 等）信息。
  2. 当 Slave 接收到来自 Master 的初始化量子线路后，会再当前的slave服务器上随机选择一个开用的端口开启一个work服务，并建立RPC连接。
  3. Slave 服务同样维护一份和 Master 一样的量子线路任务请求列表。

##### Work Service:

  1. Work 服务实现了量子线路中各种门的操作，包括 *H, CH, P, CP, R, CR, Rx, Ry, Rz, Rxx, Ryy, Rzz, X, Y, Z, S, T, Sdg, Tdg, SqrtX, CSqrtX, SqrtSwap, Swap, CSwap, CNot, MCX, CY, MCZ, U1, U2, U3, U, CU, ISwap, SqrtXdg, PH*
  2. Work 服务的各种门操作是基于QuEST量子模拟计算平台的基础上改进和演化的。


### 编译，安装，启动服务
---
##### 编译:

1. QuSprout 根目录下的thirdparty目录里面包含了所有编译和运行QuSprout的第三方库，该目录下的install.sh脚本可以一键安装所有的依赖库:
   ```Shell
    chmod a+x install.sh
    ./install.sh
    ```
1. QuSprout 根目录下有个 build_qusprout.sh 脚本文件，这个脚本文件是用来对 QuSprout 进行编译的。
2. 赋予编译脚本执行权限：
    ```Shell
    chmod a+x build_qusprout.sh
    ```
3. 执行编译脚本，0.1.11是需要编译的版本号:
    ```Shell
    build_qusprout.sh version 0.1.11
    ```

##### 安装：

1. 编译完成后，会在当前目前下生成 build_v0.1.11.tar.gz 文件，0.1.11 是编译时指定的版本号。
2. QuSprout 根目录下有个 service_qusprout.sh 脚本文件，这个脚本文件是用来对 QuSprout 进行安装部署的。
3. 赋予部署脚本执行权限：
    ```Shell
    chmod a+x service_qusprout.sh
    ```
4. 执行部署脚本, 执行时会在当前目前下查找 build_v0.1.11.tar.gz 编译文件，0.1.11 是要安装部署的版本号。 
    ```Shell
    service_qusprout.sh version 0.1.11
    ```

##### 启动：

1. 安装完成后，需要启动 Master，Slave 服务。
2. 启动 Master 服务：
    ```Shell
    service_qusprout.sh start qusprout-master
    ```
3. 启动 Slave 服务：
    ```Shell
    service_qusprout.sh start qusprout-slave
    ```
4. service_qusprout 支持两外两个参数 stop，restart，分别对应于停止和重启相应的服务。


### **文档**
---
* [QuSprout API](http://developer.queco.cn/qutrunk_api/)
* [QuSprout Config](http://developer.queco.cn/qutrunk_api/)
* [QuSprout Docker](http://developer.queco.cn/qutrunk_api/)


### **如何参与开发**
---
1. 阅读源代码，了解我们当前的开发方向
2. 找到自己感兴趣的功能或模块
3. 进行开发，开发完成后自测功能是否正确
4. Fork 代码库，将修复代码提交到 fork 的代码库
5. 发起 pull request
6. 更多详情请参见[链接](./CONTRIBUTING.md)


### **许可证**
---
QuSprout 是自由和开源的，在 Apache 2.0 许可证版本下发布。
