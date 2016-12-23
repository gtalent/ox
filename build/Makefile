all: gba_build native_build native_debug_build

gba_build:
	@if [ -d gba ]; then \
		make -C gba ${ARGS}; \
	fi

native_build:
	@if [ -d release ]; then \
		make -C release ${ARGS}; \
	fi

native_debug_build:
	@if [ -d debug ]; then \
		make -C debug ${ARGS}; \
	fi
