cmake_minimum_required(VERSION 2.8)

project(WombatFS)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/Modules)
include(address_sanitizer)

if(NOT MSVC)
	add_definitions(
		-std=c++11
		-Wall
		-nostdlib
		-fno-exceptions
		-fno-rtti
		-Wsign-compare
		#-Werror
		#--analyze
		#-Os # GCC size optimization flag
	)
endif(NOT MSVC)

enable_testing()

include_directories("src")

add_subdirectory(src)
add_subdirectory(test)

add_test("Format" test/Format)
add_test("FileStoreIO" test/FileStoreIO)
