cmake_minimum_required(VERSION 2.8.8)

add_executable(
	MemFsTest
		test.cpp
)

target_link_libraries(MemFsTest Memphis)
