/*
 * Copyright 2015 - 2016 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include <_strops.hpp>
#include <filestore.hpp>

using namespace wombat::fs;

int main() {
	const auto size = 1 << 16;
	uint8_t volume[size];
	char out[6];
	uint32_t err;
	FileStore32::format(volume, size);
	FileStore32 fs(volume, volume + size, &err);
	uint32_t outSize;

	if (fs.write(1, (void*) "Hello", 6) ||
		 fs.read(1, (char*) out, &outSize) ||
		 strcmp("Hello", out)) {
		return 1;
	}

	if (fs.write(2, (void*) "World", 6) ||
		 fs.read(2, (char*) out, nullptr) ||
		 strcmp("World", out)) {
		return 1;
	}

	if (fs.read(1, (char*) out, &outSize) ||
		 strcmp("Hello", out)) {
		return 1;
	}


	return 0;
}
