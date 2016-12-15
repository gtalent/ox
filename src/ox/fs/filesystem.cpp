/*
 * Copyright 2015 - 2016 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include "filesystem.hpp"

namespace ox {
namespace fs {

FileSystem *createFileSystem(void *buff) {
	auto type = *((uint32_t*) buff);
	FileSystem *fs = nullptr;

	switch (type) {
		case ox::fs::OxFS_16:
			fs = new FileSystem16(buff);
			break;
		case ox::fs::OxFS_32:
			fs = new FileSystem32(buff);
			break;
		case ox::fs::OxFS_64:
			fs = new FileSystem64(buff);
			break;
	}

	return fs;
}

}
}
