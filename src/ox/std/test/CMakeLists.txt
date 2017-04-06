cmake_minimum_required(VERSION 2.8)

add_executable(
	StdTest
		tests.cpp
)

target_link_libraries(StdTest OxStd)

add_test("Test\\ ox_memcmp" StdTest "ox_memcmp")
