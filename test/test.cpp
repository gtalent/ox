/*
 * Copyright 2015 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include <filestore.hpp>

using namespace wombat::fs;

int main() {
	const auto size = 1 << 16;
	uint8_t volume[size];
	uint32_t err;
	initFs(volume, size, false);
	FileStore(volume, volume + size, &err);
	return err;
}
