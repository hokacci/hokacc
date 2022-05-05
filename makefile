.PHONY: setup build build-in-container build-local clean devenv-init devenv-up devenv-down devenv-remove


PROJECT_NAME = hokacc
IMAGE_NAME = ${PROJECT_NAME}-image
DEVENV_CONTAINER_NAME = ${PROJECT_NAME}-devenv

build: build-in-container

build-in-container:
	docker run --rm \
	-v $(shell pwd):/workspace \
	-v /etc/group:/etc/group:ro \
	-v /etc/passwd:/etc/passwd:ro \
	-u $(shell id -u ${USER}):$(shell id -g ${USER}) \
	${IMAGE_NAME} make build-local

build-local:
	mkdir -p build
	cd build && \
	cmake .. && \
	$(MAKE) -j

test: test-in-container

test-in-container:
	docker run --rm \
	-v $(shell pwd):/workspace \
	-v /etc/group:/etc/group:ro \
	-v /etc/passwd:/etc/passwd:ro \
	-u $(shell id -u ${USER}):$(shell id -g ${USER}) \
	${IMAGE_NAME} make test-local

test-local:
	test/test.py

setup:
	cd docker && docker build . -t ${IMAGE_NAME}

devenv-init:
	docker run \
	-v $(shell pwd):/workspace \
	-v /etc/group:/etc/group:ro \
	-v /etc/passwd:/etc/passwd:ro \
	-u $(shell id -u ${USER}):$(shell id -g ${USER}) \
	--name ${DEVENV_CONTAINER_NAME} \
	-itd \
	${IMAGE_NAME} /bin/sh
	docker container exec -u root ${DEVENV_CONTAINER_NAME} /bin/sh -c "mkdir -p /home/${USER} && chown ${USER}:${USER} /home/${USER}"

devenv-up:
	docker start ${DEVENV_CONTAINER_NAME}

devenv-down:
	docker stop ${DEVENV_CONTAINER_NAME}

devenv-remove:
	docker container rm ${DEVENV_CONTAINER_NAME}

clean:
	rm -r build tmp tmp.s
