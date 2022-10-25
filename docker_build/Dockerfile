FROM ubuntu:22.10

MAINTAINER liubo@qudoor.cn

ARG DATE
ARG work_dir=/app
ARG GIT_USERNAME=liubo
ARG GIT_PASSWD=lb\$198622

WORKDIR ${work_dir}

RUN sed -i s@/archive.ubuntu.com/@/mirrors.aliyun.com/@g /etc/apt/sources.list \
	&& sed -i s@/security.ubuntu.com/@/mirrors.aliyun.com/@g /etc/apt/sources.list \
	&& apt-get clean \
	&& apt-get update \
	&& apt-get install -y gcc \
	&& apt-get install -y g++ \
	&& apt-get install -y unzip git wget ca-certificates make python2 python3 \
	&& apt-get clean \
    && git config --global http.lowSpeedLimit 0 && git config --global http.lowSpeedTime 999999 && git config --global http.postBuffer 5242880 \
    && git clone http://${GIT_USERNAME}:"${GIT_PASSWD}"@192.168.170.196/qudoor/qusprout.git /app/qusprout \
	&& cd /app/qusprout/thirdparty && chmod +x install.sh && ./install.sh \
	&& rm -rf /app/qusprout