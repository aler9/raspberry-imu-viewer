
all: build_standard

build_standard:
	@$(MAKE) -f Makefile.src

build_cross:
	docker run --rm --privileged multiarch/qemu-user-static:register --reset --credential yes >/dev/null
	docker build . -t temp

TARGET ?= pi@192.168.2.170

deploy: build_cross
	ssh $(TARGET) "killall raspberry-imu-viewer || exit 0"
	docker run --rm temp cat /src/raspberry-imu-viewer \
	| ssh $(TARGET) "tee ./raspberry-imu-viewer >/dev/null"
	ssh $(TARGET) "chmod +x ./raspberry-imu-viewer"
	ssh $(TARGET) "./raspberry-imu-viewer"
