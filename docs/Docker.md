# QuSprout镜像编译文档
## 1 概述
文档对QuSprout的编译镜像，服务镜像，以及镜像的运行进行了详细的说明。

## 2 QuSprout的docker安装
### 2.1 安装docker
可以通过官网链接安装：https://docs.docker.com/get-docker/

### 2.2 编译镜像编译
根目录下的docker_build目录包含的Dockerfile用来安装编译环境并生成编译镜像。
```
cd docker_build
docker build -t qusprout_ubuntu:1.0.0 .
```

### 2.3 服务镜像编译
编译镜像生成后，根目录下的docker目录包含的Dockerfile用来编译QuSprout程序并生成服务镜像，该服务镜像包含安装部署好的qusprout服务。
```
cd docker
docker build -t qusprout:1.0.0 .
```

### 2.4 运行镜像
启动服务镜像后，服务容器里的qusprout服务自动启动，能接收处理量子线路。

```
docker run -p 9091:9091 -p 9093:9093 -d --name qusprout --network host qusprout:1.0.0
```