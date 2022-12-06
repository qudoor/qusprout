## QuSprout FAQ


下面列出了QuSprout目前的一些热点问题和已知问题：

#### Q1：为什么要开源？

Ans：邀请更多的量子计算爱好者参与到量子模拟计算中来，丰富量子模拟计算功能，提高量子模拟计算性能。

#### Q2：CentOS7系列通过install.sh一键安装依赖库有可能失败？

Ans：因为CentOS7上自带的一些库版本比较低，例如：gcc，openssl，automake，libtool，会导致版本不兼容的问题，用户能手动升级这些库或者通过yum remove后，重新执行[install.sh](./thirdparty/install.sh)。

#### Q3：怎么样编译，安装和启动QuSprout？

Ans：README.md文档中详细描述了编译，安装，启动qusprout的方法。

#### Q4：怎么样使用docker？

Ans：Docker.md文档中详细描述了生成编译镜像和服务镜像的方法。






