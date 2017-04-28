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
	int Int = 0;
	bstring<32> String = "";
};

struct TestStruct {
	bool Bool = false;
	int32_t Int = 0;
	bstring<32> String = "";
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
	io->setFields(4);
	err |= io->op("Bool", &obj->Bool);
	err |= io->op("Int", &obj->Int);
	err |= io->op("String", &obj->String);
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
				uint8_t buff[buffLen];
				TestStruct ts;
				return write(buff, buffLen, &ts);
			}
		},
		{
			"MetalClawReader",
			[](string) {
				int err = 0;
				size_t buffLen = 1024;
				uint8_t buff[buffLen];
				TestStruct testIn, testOut;

				testIn.Bool = true;
				testIn.Int = 42;
				testIn.String = "Test String 1";
				testIn.Struct.Bool = false;
				testIn.Struct.Int = 300;
				testIn.Struct.String = "Test String 2";

				err |= write(buff, buffLen, &testIn);
				err |= read(buff, buffLen, &testOut);

				err |= !(testIn.Bool          == testOut.Bool);
				err |= !(testIn.Int           == testOut.Int);
				err |= !(testIn.String        == testOut.String);
				err |= !(testIn.Struct.Bool   == testOut.Struct.Bool);
				err |= !(testIn.Struct.Int    == testOut.Struct.Int);
				err |= !(testIn.Struct.String == testOut.Struct.String);

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
