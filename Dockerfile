FROM alpine:3.20

# deps
RUN apk update && \
    apk add --no-cache \
        git \
        gcc \
        libc-dev \
        curl \
        curl-dev \
        libcurl \
        make \
        musl-dev \
        gettext

# concord
WORKDIR /concord
RUN git clone https://github.com/Cogmasters/concord.git /concord
RUN cd /concord
RUN make
RUN make install
RUN cd / && \
    rm -rf /concord

WORKDIR /app

COPY ./src ./src
COPY Makefile .

RUN mkdir build
RUN make

COPY config.json .

ENTRYPOINT ["/bin/sh", "-c", "envsubst < /app/config.json > /app/config_resolved.json && /app/build/bot /app/config_resolved.json"]
