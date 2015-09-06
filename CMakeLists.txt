cmake_minimum_required(VERSION 2.8.8)

project(Memphis)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/Modules)
include(address_sanitizer)

add_definitions(
	-std=c++11
	-Wall
	-Wsign-compare
	-nostdlib
	#-Werror
	#--analyze
	#-Os # GCC size optimization flag
)

add_subdirectory(src)
