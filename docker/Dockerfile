FROM ubuntu_server_qusprout:latest

ENV QUSPROUT_VERSION=0.1.9
COPY ./service_qusprout.sh ./start_qusprout.sh ./build-v${QUSPROUT_VERSION}.tar.gz /tmp/install/

RUN cd /tmp/install \
    && ./service_qusprout.sh version ${QUSPROUT_VERSION} \
    && rm -rf /tmp/install/build-v${QUSPROUT_VERSION}.tar.gz

ENTRYPOINT ["sh", "/tmp/install/start_qusprout.sh"]



