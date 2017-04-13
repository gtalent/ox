/*
 * Copyright 2015 - 2017 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include "filesystem.hpp"

namespace ox {
namespace fs {

FileSystem *createFileSystem(void *buff, size_t buffSize) {
	auto version = ((FileStore16*) buff)->version();
	auto type = ((FileStore16*) buff)->fsType();
	FileSystem *fs = nullptr;

	switch (version) {
		case 4:
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
			break;
		default:
			break;
	}

	if (fs->size() > buffSize) {
		delete fs;
		fs = nullptr;
	}

	return fs;
}

}
}
