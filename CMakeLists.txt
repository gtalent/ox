cmake_minimum_required(VERSION 2.8.8)

project(Memphis)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/Modules)
include(address_sanitizer)

add_definitions(
	-std=c++11
	-Wall
	-Wsign-compare
	-nostdlib
	-fno-exceptions
	-fno-rtti
	#-Werror
	#--analyze
	#-Os # GCC size optimization flag
)

enable_testing()

include_directories("src")

add_subdirectory(src)
add_subdirectory(test)

add_test("MemFs" test/MemFsTest)
