/*
 * Copyright 2015 - 2017 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include <iostream>
#include <map>
#include <functional>
#include <ox/std/std.hpp>

using namespace std;
using namespace ox::std;

template<typename T>
int testLittleEndianToNative(string str) {
	auto i = (T) stoul(str, nullptr, 16);
	return !(littleEndianToNative(littleEndianToNative(i)) == i);
}

map<string, function<int(string)>> tests = {
	{
		{ "littleEndianToNative<uint16_t>", testLittleEndianToNative<uint16_t> },
		{ "littleEndianToNative<uint32_t>", testLittleEndianToNative<uint32_t> },
		{ "littleEndianToNative<uint64_t>", testLittleEndianToNative<uint64_t> },
	},
};

int main(int argc, const char **args) {
	if (argc > 1) {
		auto testName = args[1];
		auto testArg = args[2];
		if (tests.find(testName) != tests.end()) {
			return tests[testName](testArg);
		}
	}
	return -1;
}
