cmake_minimum_required(VERSION 2.8)

add_library(
	OxFS
		filesystem.cpp
)

if(OX_BUILD_EXEC STREQUAL "ON")
	add_executable(
		oxfstool
			oxfstool.cpp
	)
endif()

set_target_properties(oxfstool PROPERTIES OUTPUT_NAME oxfs)
target_link_libraries(oxfstool OxFS OxStd)

install(
	FILES
		filestore.hpp
		filesystem.hpp
		inodemgr.hpp
	DESTINATION
		include/ox/fs
)

install(TARGETS oxfstool OxFS
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
)

if(OX_BUILD_EXEC STREQUAL "ON")
	add_subdirectory(test)
endif()
