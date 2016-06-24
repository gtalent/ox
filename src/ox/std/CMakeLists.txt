cmake_minimum_required(VERSION 2.8)

add_library(
	OxStd
		_memops.cpp
		_strops.cpp
)

install(
	FILES
		_memops.hpp
		_strops.hpp
	DESTINATION
		include/ox/std
)
