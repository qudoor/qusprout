## QuSprout量子计算模拟平台

[![Documentation Status](https://img.shields.io/badge/docs-latest-brightgreen.svg)](docs/ThriftApi.md)
[![License](https://img.shields.io/badge/license-Apache%202-blue.svg)](LICENSE)
[![Download Code](https://img.shields.io/badge/download-zip-green.svg)](https://github.com/queco-quantum/qusprout/archive/refs/heads/main.zip)


### **概述**
---
* QuSprout 是启科量子自主研发的一款免费、开源的量子计算模拟后端，用户在 QuTrunk 量子编程框架生成量子线路后, 如果需要更高的运行效率，需要连接到 QuSprout 计算后端来进行模拟计算。
* QuSprout 支持多个量子线路的任务管理、单机MPI多进程并行计算。
* QuSprout 使用 C++ 作为宿主语言，当前支持在 Ubuntu，CentOS系列上编译部署，建议使用Ubuntu22.04，CentOS7.9，后续会对MacOS系列支持。
* QuSprout 提供了 qusprout，quwork 服务模块来支持量子计算的模拟：
    * qusprout 服务通过 RPC 与 QuTrunk 量子编程框架连接，主要负责量子线路的任务管理、quwork 服务的启动等。
    * quwork 服务主要负责量子线路的计算并返回计算结果。


### **服务模块**
---
##### qusprout:

  1. qusprout 服务启动后，会先收集本机设备信息，包括主机信息(主机名，发布版本等)和资源信息(总内存，未使用内存等)，并开启RPC服务，监听来自 QuTrunk 量子编程框架发送的量子线路操作事件。
  2. 当 qusprout 接收到来自 QuTrunk 量子线路的初始化请求时，会先检查是否有足够的资源来运行该量子线路（通过计算量子线路的qubits 所需的内存）。
  3. 如果资源要求满足，qusprout 会将量子线路加入到任务列表，qusprout 会维护一个任务列表来管理所有的量子线路请求。然后开启 quwork 服务，并将量子线路的初始化请求发送到 quwork 服务。
  4. qusprout 会定时清理一些长时间没有更新的任务，具体超时配置在 qusprout 服务配置文件中，更多配置相关的内容请参考[配置文件](./docs/Config.md)

##### quwork:

  1. quwork 服务实现了量子线路中各种门的操作，包括 *H, CH, P, CP, R, CR, Rx, Ry, Rz, Rxx, Ryy, Rzz, X, Y, Z, S, T, Sdg, Tdg, SqrtX, CSqrtX, SqrtSwap, Swap, CSwap, CNot, MCX, CY, MCZ, U1, U2, U3, U, CU, ISwap, SqrtXdg, PH*
  2. quwork 服务的各种门操作是基于QuEST量子模拟计算平台的基础上改进和演化的。


### 编译，安装，启动服务
---
##### 编译:

1. QuSprout 根目录下的thirdparty目录里面包含了大部分编译和运行 QuSprout 的第三方库，该目录下的install.sh脚本可以一键安装依赖库:
   ```Shell
    sudo chmod a+x install.sh
    sudo ./install.sh
    Noted： 1. CentOS上需要使用root权限执行install.sh脚本。
            2. 执行install.sh之前需要提前安装基本的开发工具包括gcc，g++, make, unzip。
            3. 执行install.sh过程中，可能在不同的Linux平台会遇到个别依赖库安装失败的情况，您可以尝试解决或者在github上创建 Issue。
    ```
2. QuSprout 根目录下有个 build_qusprout.sh 脚本文件，这个脚本文件是用来对 QuSprout 进行编译的。
3. 赋予编译脚本执行权限：
    ```Shell
    sudo chmod a+x build_qusprout.sh
    ```
4. 执行编译脚本，0.1.12是需要指定的编译版本号，您可以指定任意的版本号:
    ```Shell
    sudo ./build_qusprout.sh version 0.1.12
    ```

##### 安装：

1. 编译完成后，会在当前目前下生成 build_v0.1.12.tar.gz 文件，0.1.12 是编译时指定的版本号。
2. QuSprout 根目录下有个 service_qusprout.sh 脚本文件，这个脚本文件是用来对 QuSprout 进行安装部署的。
3. 赋予部署脚本执行权限：
    ```Shell
    sudo chmod a+x service_qusprout.sh
    ```
4. 执行部署脚本, 执行时会在当前目前下查找 build_v0.1.12.tar.gz 编译文件，0.1.12 是要安装部署的版本号。 
    ```Shell
    sudo ./service_qusprout.sh version 0.1.12
    ```

##### 启动：

1. 安装完成后，需要启动 qusprout 服务。
2. 启动 qusprout 服务：
    ```Shell
    sudo ./service_qusprout.sh start qusprout
    ```
3. service_qusprout 支持另外两个参数 stop，restart，分别对应于停止和重启服务。


### **文档**
---
* [QuSprout API](./docs/ThriftApi.md)
* [QuSprout Config](./docs/Config.md)
* [QuSprout Docker](./docs/Docker.md)


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
