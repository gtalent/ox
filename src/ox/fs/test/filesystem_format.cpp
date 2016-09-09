/*
 * Copyright 2015 - 2016 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include <ox/fs/filesystem.hpp>
#include <ox/fs/inodemgr.hpp>

using namespace ox::fs;
using namespace ox::std;

template<typename FileSystem>
int test() {
	const uint16_t size = ~0;
	uint8_t volume[size];
	FileSystem::format(volume, size, true);
	return 0;
}

int main() {
	return test<FileSystem16>() | test<FileSystem32>() | test<FileSystem64>();
}
