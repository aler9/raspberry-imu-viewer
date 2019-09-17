
all: build_direct

build_direct:
	@$(MAKE) -f Makefile.src

build_cross:
	docker run --rm --privileged multiarch/qemu-user-static:register --reset --credential yes >/dev/null
	docker build . -t temp

TARGET ?= pi@192.168.2.170
BIN = raspberry-imu-viewer

deploy: build_cross
	ssh $(TARGET) "killall $(BIN) || exit 0"
	docker run --rm temp cat /s/$(BIN) \
	| ssh $(TARGET) "tee ./$(BIN) >/dev/null"
	ssh $(TARGET) "chmod +x ./$(BIN)"
	ssh $(TARGET) "./$(BIN)"
