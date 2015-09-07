cmake_minimum_required(VERSION 2.8.8)

add_library(
	Memphis
		memfs.cpp
		_memops.cpp
)

install(
	FILES
		memfs.hpp
		_memops.hpp
	DESTINATION
		include/Memphis
)
