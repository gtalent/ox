cmake_minimum_required(VERSION 2.8.8)

add_executable(
	Format
		format.cpp
)

add_executable(
	FsStore
		fsstore.cpp
)

target_link_libraries(Format WFS)
target_link_libraries(FsStore WFS)
