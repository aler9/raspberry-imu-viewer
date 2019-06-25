FROM balenalib/raspberry-pi-debian:stretch

RUN apt-get update && apt-get install -y --no-install-recommends \
    make \
    gcc \
    libc6-dev \
    libi2c-dev \
    libraspberrypi-dev

WORKDIR /src

COPY *.h *.c Makefile Makefile.src ./

RUN make build_standard
