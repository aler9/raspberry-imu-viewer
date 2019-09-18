
all: build_direct

build_direct:
	@[ -f "sensor-imu/imu_auto.c" ] || git submodule update --init
	@$(MAKE) -f Makefile.src

define DOCKERFILE
FROM balenalib/raspberry-pi-debian:stretch
RUN apt-get update && apt-get install -y --no-install-recommends \
    make \
    gcc \
    libc6-dev \
    libi2c-dev \
    libraspberrypi-dev
WORKDIR /s
COPY *.h *.c Makefile Makefile.src ./
COPY sensor-imu ./sensor-imu
RUN make build_direct
endef
export DOCKERFILE

build_cross:
	@[ -f "sensor-imu/imu_auto.c" ] || git submodule update --init
	docker run --rm --privileged multiarch/qemu-user-static:register --reset --credential yes >/dev/null
	echo "$$DOCKERFILE" | docker build . -f - -t temp

TARGET ?= pi@192.168.2.170
BIN = raspberry-imu-viewer

deploy: build_cross
	ssh $(TARGET) "killall $(BIN) || exit 0"
	docker run --rm temp cat /s/$(BIN) \
	| ssh $(TARGET) "tee ./$(BIN) >/dev/null"
	ssh $(TARGET) "chmod +x ./$(BIN)"
	ssh $(TARGET) "./$(BIN)"
