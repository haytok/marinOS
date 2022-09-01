DOCKER_IMAGE_NAME=marinos
DOCKER_CONTAINER_NAME=marinos

.PHONY: build
build:
	docker build -t ${DOCKER_IMAGE_NAME} .

.PHONY: run
run:
	docker run \
		--name ${DOCKER_CONTAINER_NAME} \
		--rm \
		-it \
		--privileged \
		-v /dev/ttyUSB0:/dev/ttyUSB0 \
		-v ${PWD}:${HOME} \
		${DOCKER_IMAGE_NAME} \
		/bin/bash

.PHONY: down
down:
	docker stop ${DOCKER_CONTAINER_NAME}
