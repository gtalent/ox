/*
 * Copyright 2015 - 2016 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include <iostream>
#include <map>
#include <functional>
#include <ox/std/std.hpp>

::std::map<std::string, std::function<int()>> tests = {
	{
		"ox_memcmp",
		[]() {
			int success = 1;
			const char *data1 = "ABCDEFG";
			const char *data2 = "HIJKLMN";
			success &= ox_memcmp(data1, data2, 7) < 0;
			success &= ox_memcmp(data2, data1, 7) > 0;
			success &= ox_memcmp(data1, data1, 7) == 0;
			return !success;
		}
	}
};

int main(int argc, const char **args) {
	if (argc > 1) {
		auto testName = args[1];
		if (tests.find(testName) != tests.end()) {
			return tests[testName]();
		}
	}
	return -1;
}
