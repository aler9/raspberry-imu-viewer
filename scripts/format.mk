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
