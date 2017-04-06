cmake_minimum_required(VERSION 2.8)

add_library(
	OxStd
		memops.cpp
		strops.cpp
)

install(
	FILES
		memops.hpp
		strops.hpp
		std.hpp
		types.hpp
	DESTINATION
		include/ox/std
)

if(OX_BUILD_EXEC STREQUAL "ON")
	add_subdirectory(test)
endif()
