FROM ubuntu:22.10

MAINTAINER qudoor

ARG DATE
ARG work_dir=/app

WORKDIR ${work_dir}

RUN sed -i s@/archive.ubuntu.com/@/mirrors.aliyun.com/@g /etc/apt/sources.list \
	&& sed -i s@/security.ubuntu.com/@/mirrors.aliyun.com/@g /etc/apt/sources.list \
	&& apt-get clean \
	&& apt-get update \
	&& apt-get install -y gcc \
	&& apt-get install -y g++ \
	&& apt-get install -y unzip git wget ca-certificates make python2 python3 \
	&& apt-get install -y libudev-dev zlib1g-dev kmod udev \
	&& apt-get clean \
    && git config --global http.lowSpeedLimit 0 && git config --global http.lowSpeedTime 999999 && git config --global http.postBuffer 5242880 \
    && git clone https://github.com/qudoor/qusprout.git /app/qusprout \
	&& cd /app/qusprout/thirdparty && chmod +x install.sh && ./install.sh \
	&& rm -rf /app/qusprout