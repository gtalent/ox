/*
 * Copyright 2015 - 2016 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef WOMBAT_FS_FILESYSTEM_HPP
#define WOMBAT_FS_FILESYSTEM_HPP

#include "_memops.hpp"
#include "_strops.hpp"
#include "_types.hpp"
#include "filestore.hpp"

namespace wombat {
namespace fs {

template<typename FileStore>
class FileSystem {

	private:
		FileStore *store = nullptr;
};

typedef FileSystem<FileStore16> FileSystem16;
typedef FileSystem<FileStore32> FileSystem32;
typedef FileSystem<FileStore64> FileSystem64;

}
}

#endif
