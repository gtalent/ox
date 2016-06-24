cmake_minimum_required(VERSION 2.8)

add_executable(
	Format
		format.cpp
)

add_executable(
	FileStoreIO
		filestoreio.cpp
)

target_link_libraries(Format OxFS OxStd)
target_link_libraries(FileStoreIO OxFS OxStd)

add_test("Format" Format)
add_test("FileStoreIO" FileStoreIO)
