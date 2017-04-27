cmake_minimum_required(VERSION 2.8)

add_library(
	OxMetalClaw
		read.cpp
		write.cpp
)

set_property(
	TARGET
		OxMetalClaw
	PROPERTY
		POSITION_INDEPENDENT_CODE ON
)

install(
	FILES
		read.hpp
		write.hpp
	DESTINATION
		include/ox/mc
)

install(TARGETS OxMetalClaw
        LIBRARY DESTINATION lib/ox
        ARCHIVE DESTINATION lib/ox
)

if(OX_RUN_TESTS STREQUAL "ON")
	add_subdirectory(test)
endif()
