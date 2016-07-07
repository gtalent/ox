cmake_minimum_required(VERSION 2.8)

add_library(
	OxFS
		filesystem.cpp
)

add_executable(
	oxfstool
		oxfstool.cpp
)
target_link_libraries(oxfstool OxFS OxStd)

install(
	FILES
		filestore.hpp
		filesystem.hpp
		inodemgr.hpp
	DESTINATION
		include/ox/fs
)

add_subdirectory(test)
