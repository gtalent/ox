cmake_minimum_required(VERSION 2.8)

add_executable(
	StdTest
		tests.cpp
)

target_link_libraries(StdTest OxStd)

add_test("Test\\ ox_memcmp\\ ABCDEFG\\ !=\\ HIJKLMN" StdTest "ABCDEFG != HIJKLMN")
add_test("Test\\ ox_memcmp\\ HIJKLMN\\ !=\\ ABCDEFG" StdTest "HIJKLMN != ABCDEFG")
add_test("Test\\ ox_memcmp\\ ABCDEFG\\ ==\\ ABCDEFG" StdTest "ABCDEFG == ABCDEFG")
add_test("Test\\ ox_memcmp\\ ABCDEFGHI\\ ==\\ ABCDEFG" StdTest "ABCDEFGHI == ABCDEFG")


################################################################################
# StrOps Tests

add_executable(
	StrOpsTest
		strops_test.cpp
)

target_link_libraries(StrOpsTest OxStd)

add_test("Test\\ ox_strcmp\\ asdf\\ !=\\ hijk" StrOpsTest "asdf < hijk")
add_test("Test\\ ox_strcmp\\ hijk\\ !=\\ asdf" StrOpsTest "hijk > asdf")
add_test("Test\\ ox_strcmp\\ read\\ !=\\ resize" StrOpsTest "read < resize")
add_test("Test\\ ox_strcmp\\ resize\\ !=\\ read" StrOpsTest "resize > read")
add_test("Test\\ ox_strcmp\\ resize\\ ==\\ resize" StrOpsTest "resize == resize")


################################################################################
# Byte Swap Tests

add_executable(
	ByteSwapTest
		byteswap_test.cpp
)

target_link_libraries(ByteSwapTest OxStd)

add_test("Test\\ nativizeLittleEndian\\ 40" ByteSwapTest "40")
