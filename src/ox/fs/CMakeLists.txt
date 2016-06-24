cmake_minimum_required(VERSION 2.8)

add_library(
	OxFS
		filestore.cpp
		filesystem.cpp
)

install(
	FILES
		filestore.hpp
		filesystem.hpp
	DESTINATION
		include/ox/fs
)

add_subdirectory(test)
