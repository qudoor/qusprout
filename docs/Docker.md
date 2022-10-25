# QuSprout镜像编译文档
## 1 概述
文档对QuSprout的编译镜像，服务镜像，以及镜像的运行进行了详细的说明。

## 2 QuSprout的docker安装
### 2.1 安装docker
```
curl -fsSL https://get.docker.com | bash -s docker --mirror Aliyun
```

### 2.2 编译镜像编译
```
cd docker_build
docker build -t qusprout_ubuntu:1.0.0 .
```

### 2.3 服务镜像编译
```
cd docker
docker build -t qusprout:1.0.0 .
```

### 2.4 运行镜像
```
docker run -p 9091:9091 -p 9093:9093 -d --name qusprout --network host qusprout:1.0.0
```