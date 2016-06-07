cmake_minimum_required(VERSION 2.8.8)

add_library(
	WFS
		filestore.cpp
		_memops.cpp
		_strops.cpp
)

install(
	FILES
		filestore.hpp
		_memops.hpp
	DESTINATION
		include/WFS
)
