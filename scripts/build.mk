build-direct:
	[ -n "$$IN_DOCKER" ] || git submodule status | grep -vq '^-' || git submodule update --init --recursive
	@$(MAKE) -f Makefile.src

define DOCKERFILE_BUILD
FROM $(RPI_IMAGE)
RUN apt-get update && apt-get install -y --no-install-recommends \
	make \
	gcc \
	libc6-dev \
	libi2c-dev \
	libraspberrypi-dev \
	libegl-dev \
	libgles-dev
WORKDIR /s
COPY . ./
COPY sensor-imu ./sensor-imu
ENV IN_DOCKER 1
RUN make build-direct
endef
export DOCKERFILE_BUILD

build-cross:
	git submodule status | grep -vq '^-' || git submodule update --init --recursive
	docker run --rm --privileged multiarch/qemu-user-static:register --reset --credential yes >/dev/null
	echo "$$DOCKERFILE_BUILD" | docker build . -f - -t temp
