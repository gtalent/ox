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

map<string, function<int()>> tests = {
	{
		"asdf < hijk",
		[]() {
			return !(ox_strcmp("asdf", "hijk") < 0);
		}
	},
	{
		"hijk > asdf",
		[]() {
			return !(ox_strcmp("hijk", "asdf") > 0);
		}
	},
	{
		"resize > read",
		[]() {
			return !(ox_strcmp("resize", "read") > 0);
		}
	},
	{
		"read < resize",
		[]() {
			return !(ox_strcmp("read", "resize") < 0);
		}
	},
	{
		"resize == resize",
		[]() {
			return !(ox_strcmp("resize", "resize") == 0);
		}
	},
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
