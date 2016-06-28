cmake_minimum_required(VERSION 2.8)

add_library(
	OxFS
		filestore.cpp
		filesystem.cpp
		inodemgr.cpp
)

install(
	FILES
		filestore.hpp
		filesystem.hpp
		inodemgr.hpp
	DESTINATION
		include/ox/fs
)

add_subdirectory(test)
