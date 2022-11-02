FROM qusprout_ubuntu:1.0.0 as builder

MAINTAINER liubo@qudoor.cn

ARG DATE
ARG work_dir=/app
ARG GIT_USERNAME=liubo
ARG GIT_PASSWD=lb\$198622

WORKDIR ${work_dir}

RUN git config --global http.lowSpeedLimit 0 && git config --global http.lowSpeedTime 999999 && git config --global http.postBuffer 5242880 \
    && git clone http://${GIT_USERNAME}:"${GIT_PASSWD}"@192.168.170.196/qudoor/qusprout.git /app/qusprout \
	&& cd /app/qusprout/thirdparty && chmod +x *.sh && ./install_QuEST.sh && cd /app/qusprout/thirdparty && rm -rf QuEST-3.5.0 \
	&& cd /app/qusprout && chmod +x *.sh && ./build_qusprout.sh version 1.0 && ./service_qusprout.sh version 1.0 \
	&& rm -rf /app/qusprout
	
FROM ubuntu:22.10
MAINTAINER liubo <liubo@qudoor.cn>

WORKDIR /app

RUN mkdir -p /usr/local/bin && mkdir -p /usr/local/etc/qusprout
	
COPY --from=builder /usr/local/bin/qusprout /usr/local/bin/
COPY --from=builder /usr/local/bin/quwork /usr/local/bin/
COPY --from=builder /usr/local/etc/qusprout/qusprout.yaml /usr/local/etc/qusprout/
COPY --from=builder /usr/local/etc/qusprout/quwork.yaml /usr/local/etc/qusprout/


ENV LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/app:/usr/lib:/usr/lib64:/usr/local/lib:/usr/local/lib64

ENTRYPOINT ["sh", "/usr/local/bin/qusprout", "-c", "/usr/local/etc/qusprout/qusprout.yaml"]