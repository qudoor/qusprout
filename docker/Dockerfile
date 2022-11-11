FROM qusprout_ubuntu:1.0.0 as builder

MAINTAINER qudoor

ARG DATE
ARG work_dir=/app

WORKDIR ${work_dir}

RUN git config --global http.lowSpeedLimit 0 && git config --global http.lowSpeedTime 999999 && git config --global http.postBuffer 5242880 \
    && git clone https://github.com/qudoor/qusprout.git /app/qusprout \
	&& cd /app/qusprout/thirdparty && chmod +x *.sh && ./install_QuEST.sh && cd /app/qusprout/thirdparty && rm -rf QuEST-3.5.0 \
	&& cd /app/qusprout && chmod +x *.sh && ./build_qusprout.sh version 1.0 && ./service_qusprout.sh version 1.0 \
	&& rm -rf /app/qusprout
	
FROM ubuntu:22.10
MAINTAINER qudoor

WORKDIR /app

RUN mkdir -p /usr/local/bin && mkdir -p /usr/local/etc/qusprout \
	&& sed -i s@/archive.ubuntu.com/@/mirrors.aliyun.com/@g /etc/apt/sources.list \
	&& sed -i s@/security.ubuntu.com/@/mirrors.aliyun.com/@g /etc/apt/sources.list \
	&& apt-get clean \
	&& apt-get update \
	&& apt-get install -y libgomp1 openssh-server \ 
	&& apt-get clean
	
COPY --from=builder /usr/local/bin/mpiexec /usr/local/bin/mpiexec
COPY --from=builder /usr/local/bin/hydra_pmi_proxy /usr/local/bin/hydra_pmi_proxy
COPY --from=builder /usr/local/lib/libhwloc.so.15 /usr/local/lib/libhwloc.so.15
COPY --from=builder /usr/local/bin/qusprout /usr/local/bin/
COPY --from=builder /usr/local/bin/quwork /usr/local/bin/
COPY --from=builder /usr/local/etc/qusprout/qusprout.yaml /usr/local/etc/qusprout/
COPY --from=builder /usr/local/etc/qusprout/quwork.yaml /usr/local/etc/qusprout/


ENV LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/app:/usr/lib:/usr/lib64:/usr/local/lib:/usr/local/lib64

ENTRYPOINT ["/usr/local/bin/qusprout", "-c", "/usr/local/etc/qusprout/qusprout.yaml"]