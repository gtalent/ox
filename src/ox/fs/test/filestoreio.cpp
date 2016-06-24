/*
 * Copyright 2015 - 2016 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include <ox/std/_strops.hpp>
#include <ox/fs/filestore.hpp>

using namespace ox::fs;

template<typename FileStore>
int test() {
	const auto size = 65535;
	uint8_t volume[size];
	char out[6];
	uint32_t err;
	typename FileStore::FsSize_t outSize;
	FileStore::format(volume, size);
	FileStore fs(volume, volume + size, &err);

	if (fs.write(1, (void*) "Hello", 6) ||
		 fs.read(1, (char*) out, &outSize) ||
		 strcmp("Hello", out)) {
		return 1;
	}

	if (fs.write(2, (void*) "World", 6) ||
		 fs.read(2, (char*) out, &outSize) ||
		 strcmp("World", out)) {
		return 1;
	}

	// make sure first value was not overwritten
	if (fs.read(1, (char*) out, &outSize) ||
		 strcmp("Hello", out)) {
		return 1;
	}

	return 0;
}

int main() {
	return test<FileStore16>() | test<FileStore32>() | test<FileStore64>();
}
