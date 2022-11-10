# QuSprout Docker 使用文档
### 1 概述
QuSprout提供了编译和服务两种镜像。编译镜像包含了编译QuSprout所需的全部依赖库，服务镜像包含了安装部署好的qusprout，quwork量子模拟计算服务。

### 2 获取生成编译镜像，服务镜像
####2.1 编译镜像
* 从Dockerhub直接获取编译镜像 (**推荐**)
    * docker pull qudoor/qusprout_ubuntu:v1.0.0-amd64 从Dockerhub获取x86编译镜像
    * docker pull qudoor/qusprout_ubuntu:v1.0.0-arm64 从Dockerhub获取arm编译镜像

* 通过DockerFile生成编译镜像 (**耗时会比较长**)
    * docker_build目录中包含的Dockerfile用来安装编译环境并生成编译镜像。
        ```
        cd docker_build
        docker build -t qusprout_ubuntu:1.0.0 .
        ```
####2.2 服务镜像
* 从Dockerhub直接获取服务镜像
    * docker pull qudoor/qusprout:v1.0.0-amd64 从Dockerhub获取x86服务镜像
    * docker pull qudoor/qusprout:v1.0.0-arm64 从Dockerhub获取arm服务镜像

* 通过DockerFile生成服务镜像
    * docker目录中包含的Dockerfile用来编译QuSprout程序并生成服务镜像，该服务镜像中包含了安装部署好的qusprout，quwork量子模拟计算服务。
        ```
        cd docker
        docker build -t qusprout:1.0.0 .
        ```
        *Noted: 生成服务镜像时，基于的编译镜像既可以是从Dockerhub获取的，也可以是从DockerFile生成的，只需要在DockerFile的FROM xxx as builder语句中指定就可以了(xxx代表编译镜像)。*
    

### 3 运行镜像
启动服务镜像后，服务容器里的qusprout服务自动启动，能接收处理量子线路。

```
docker run --name qusprout -it --privileged -p 9091:9091 -p 9093:9093 -d qusprout:1.0.0
```