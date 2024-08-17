RPI_IMAGE = balenalib/raspberry-pi:bullseye
UBUNTU_IMAGE = ubuntu:22.04

all: build-direct

include scripts/*.mk
