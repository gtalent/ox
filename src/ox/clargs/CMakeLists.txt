cmake_minimum_required(VERSION 2.8)

add_library(
	OxClArgs
		clargs.cpp
)

install(
	FILES
		clargs.hpp
	DESTINATION
		include/ox/clargs
)

install(
	TARGETS
		OxClArgs
	LIBRARY DESTINATION lib/ox
	ARCHIVE DESTINATION lib/ox
)
