all: gba_build native_build native_debug_build windows_release windows_debug

gba_build:
	@if [ -d gba ]; then \
		make -C gba ${ARGS}; \
	fi

native_build:
	@if [ -d ${TARGET}-release ]; then \
		make -C ${TARGET}-release ${ARGS}; \
	fi

native_debug_build:
	@if [ -d ${TARGET}-debug ]; then \
		make -C ${TARGET}-debug ${ARGS}; \
	fi

windows_release:
	@if [ -d windows-release ]; then \
		make -C windows-release ${ARGS}; \
	fi

windows_debug:
	@if [ -d windows-debug ]; then \
		make -C windows-debug ${ARGS}; \
	fi
