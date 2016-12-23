DEVENV=devenv$(shell pwd | sed 's/\//-/g')
ifeq ($(shell docker inspect --format="{{.State.Status}}" ${DEVENV} 2>&1),running)
	ENV_RUN=docker exec --user $(shell id -u ${USER}) ${DEVENV}
endif

make:
	${ENV_RUN} make -j -C build
preinstall:
	${ENV_RUN} make -j -C build ARGS="preinstall"
install:
	${ENV_RUN} make -j -C build ARGS="install"
clean:
	${ENV_RUN} make -j -C build ARGS="clean"
purge:
	${ENV_RUN} rm -rf $(shell find build -mindepth 1 -maxdepth 1 -type d)
test:
	${ENV_RUN} make -j -C build ARGS="test"
run: make
	./build/current/src/wombat/wombat -debug
debug: make
	gdb ./build/current/src/wombat/wombat
devenv:
	docker pull wombatant/devenv
	docker run -d -v $(shell pwd):/usr/src/project \
		-e LOCAL_USER_ID=$(shell id -u ${USER}) \
		--name ${DEVENV} -t wombatant/devenv bash
devenv-destroy:
	docker rm -f ${DEVENV}

native:
	${ENV_RUN} rm -rf build/release
	${ENV_RUN} ./scripts/setup_build
	${ENV_RUN} rm -f build/current
	${ENV_RUN} ln -s release build/current

native_debug:
	${ENV_RUN} rm -rf build/debug
	${ENV_RUN} ./scripts/setup_build_debug
	${ENV_RUN} rm -f build/current
	${ENV_RUN} ln -s debug build/current

gba:
	${ENV_RUN} rm -rf build/gba
	${ENV_RUN} ./scripts/setup_build_gba
	${ENV_RUN} rm -f build/current
	${ENV_RUN} ln -s gba build/current
