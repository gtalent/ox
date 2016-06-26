/*
 * Copyright 2015 - 2016 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include <ox/fs/filesystem.hpp>

using namespace ox::fs;

template<typename FileSystem>
int test() {
	const auto size = 65535;
	uint8_t volume[size];
	FileSystem::format(volume, size);
	return 0;
}

int main() {
	return test<FileSystem16>() | test<FileSystem32>() | test<FileSystem64>();
}
