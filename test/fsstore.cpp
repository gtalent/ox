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
	FileStore32 fs(volume, volume + size, &err);

	fs.write(42, (void*) "Hello", 6);

	err = fs.read(42, (char*) out, nullptr);
	if (err) {
		return err;
	}

	err = strcmp("Hello", out);

	return err;
}
