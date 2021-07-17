
all: build-direct

build-direct:
	[ -n "$$IN_DOCKER" ] || git submodule status | grep -vq '^-' || git submodule update --init --recursive
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
ENV IN_DOCKER 1
RUN make build-direct
endef
export DOCKERFILE

build-cross:
	git submodule status | grep -vq '^-' || git submodule update --init --recursive
	docker run --rm --privileged multiarch/qemu-user-static:register --reset --credential yes >/dev/null
	echo "$$DOCKERFILE" | docker build . -f - -t temp

TARGET ?= pi@192.168.2.182
BIN = raspberry-imu-viewer

deploy: build-cross
	ssh $(TARGET) "killall $(BIN) || exit 0"
	docker run --rm temp cat /s/$(BIN) \
	| ssh $(TARGET) "tee ./$(BIN) >/dev/null"
	ssh $(TARGET) "chmod +x ./$(BIN)"
	ssh $(TARGET) "./$(BIN)"

define DOCKERFILE_FORMAT
FROM ubuntu:20.04

RUN apt update && apt install -y --no-install-recommends \
	clang-format

ENTRYPOINT [ "sh", "-c", "clang-format \
	-i \
	--style=\"{BasedOnStyle: llvm, IndentWidth: 4}\"\
	*.c \
	*.h" ]

endef
export DOCKERFILE_FORMAT

format:
	echo "$$DOCKERFILE_FORMAT" | docker build - -t temp
	docker run --rm \
	-v $(PWD):/s \
	-w /s \
	temp

define DOCKERFILE_LINT
FROM ubuntu:20.04

RUN apt update && apt install -y --no-install-recommends \
	clang-format

ENTRYPOINT [ "sh", "-c", "clang-format \
	--dry-run \
	--Werror \
	--style=\"{BasedOnStyle: llvm, IndentWidth: 4}\"\
	*.c \
	*.h" ]

endef
export DOCKERFILE_LINT

lint:
	echo "$$DOCKERFILE_LINT" | docker build - -t temp
	docker run --rm \
	-v $(PWD):/s \
	-w /s \
	temp
