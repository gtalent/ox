/*
 * Copyright 2015 - 2017 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <iostream>
#include <assert.h>
#include <map>
#include <vector>
#include <string>
#include <ox/mc/mc.hpp>
#include <ox/std/std.hpp>

using namespace std;
using namespace ox;

struct TestStructNest {
	bool Bool = false;
	uint32_t Int = 0;
	bstring<32> String = "";
};

struct TestStruct {
	bool Bool = false;
	int32_t Int = 0;
	int32_t Int1 = 0;
	int32_t Int2 = 0;
	int32_t Int3 = 0;
	int32_t Int4 = 0;
	int32_t Int5 = 0;
	int32_t Int6 = 0;
	int32_t Int7 = 0;
	int32_t Int8 = 0;
	bstring<32> String = "";
	uint32_t List[4] = {0, 0, 0 , 0};
	TestStructNest Struct;
};

template<typename T>
int ioOp(T *io, TestStructNest *obj) {
	int32_t err = 0;
	io->setFields(3);
	err |= io->op("Bool", &obj->Bool);
	err |= io->op("Int", &obj->Int);
	err |= io->op("String", &obj->String);
	return err;
}

template<typename T>
int ioOp(T *io, TestStruct *obj) {
	int err = 0;
	io->setFields(13);
	err |= io->op("Bool", &obj->Bool);
	err |= io->op("Int", &obj->Int);
	err |= io->op("Int1", &obj->Int1);
	err |= io->op("Int2", &obj->Int2);
	err |= io->op("Int3", &obj->Int3);
	err |= io->op("Int4", &obj->Int4);
	err |= io->op("Int5", &obj->Int5);
	err |= io->op("Int6", &obj->Int6);
	err |= io->op("Int7", &obj->Int7);
	err |= io->op("Int8", &obj->Int8);
	err |= io->op("String", &obj->String);
	err |= io->op("List", obj->List, 4);
	err |= io->op("Struct", &obj->Struct);
	return err;
}

map<string, int(*)(string)> tests = {
	{
		{
			"MetalClawWriter",
			[](string) {
				// This test doesn't confirm much, but it does show that the writer
				// doesn't segfault
				size_t buffLen = 1024;
				auto buff = new uint8_t[buffLen];
				int err = 0;
				TestStruct ts;

				err |= write(buff, buffLen, &ts);

				delete []buff;

				return err;
			}
		},
		{
			"MetalClawReader",
			[](string) {
				int err = 0;
				size_t buffLen = 1024;
				auto buff = new uint8_t[buffLen];
				TestStruct testIn, testOut;

				testIn.Bool = true;
				testIn.Int = 42;
				testIn.String = "Test String 1";
				testIn.List[0] = 1;
				testIn.List[1] = 2;
				testIn.List[2] = 3;
				testIn.List[3] = 4;
				testIn.Struct.Bool = false;
				testIn.Struct.Int = 300;
				testIn.Struct.String = "Test String 2";

				err |= write(buff, buffLen, &testIn);
				err |= read(buff, buffLen, &testOut);

				err |= !(testIn.Bool          == testOut.Bool);
				err |= !(testIn.Int           == testOut.Int);
				err |= !(testIn.String        == testOut.String);
				err |= !(testIn.List[0]       == testOut.List[0]);
				err |= !(testIn.List[1]       == testOut.List[1]);
				err |= !(testIn.List[2]       == testOut.List[2]);
				err |= !(testIn.List[3]       == testOut.List[3]);
				err |= !(testIn.Struct.Bool   == testOut.Struct.Bool);
				err |= !(testIn.Struct.Int    == testOut.Struct.Int);
				err |= !(testIn.Struct.String == testOut.Struct.String);

				delete []buff;

				return err;
			}
		},
	}
};

int main(int argc, const char **args) {
	int retval = -1;
	if (argc > 1) {
		auto testName = args[1];
		string testArg = "";
		if (args[2]) {
			testArg = args[2];
		}
		if (tests.find(testName) != tests.end()) {
			retval = tests[testName](testArg);
		}
	}
	return retval;
}
