cmake_minimum_required(VERSION 2.8)

add_executable(
	FileStoreFormat
		filestore_format.cpp
)

add_executable(
	FileSystemFormat
		filesystem_format.cpp
)

add_executable(
	FileStoreIO
		filestoreio.cpp
)

target_link_libraries(FileStoreFormat OxFS OxStd)
target_link_libraries(FileSystemFormat OxFS OxStd)
target_link_libraries(FileStoreIO OxFS OxStd)

add_test("FileStoreFormat" FileStoreFormat)
add_test("FileSystemFormat" FileSystemFormat)
add_test("FileStoreIO" FileStoreIO)
