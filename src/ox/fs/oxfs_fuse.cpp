/*
 * Copyright 2015 - 2017 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <string>
#include <vector>

#define _FILE_OFFSET_BITS 64
#define FUSE_USE_VERSION 30
#include <fuse.h>

#include <unistd.h>

#include <ox/log/log.hpp>
#include <ox/std/std.hpp>

#include "filesystem.hpp"
#include "toollib.hpp"
#include "oxfs_fuse.hpp"

using namespace ox;
using namespace std;

static FileSystem *fuse = nullptr;
static FileSystem *fs() {
	return fuse;
}

static const fuse_operations &fuseOps() {
	static fuse_operations ops;

	ops.getattr = [](const char *path, struct stat *out) -> int {
		debug("FUSE: getattr: %s", path);
		auto stat = fs()->stat(path);
		if (stat.inode) {
			mode_t mode;
			if (stat.fileType == FileType_Directory) {
				mode = S_IFDIR | 0755;
			} else {
				mode = S_IFREG | 0644;
			}
			out->st_mode = mode; /* permissions */
			out->st_nlink = (nlink_t) stat.links; /* number of hard links */
			out->st_uid = getuid(); /* user ID of owner */
			out->st_gid = getgid(); /* group ID of owner */
			out->st_size = (uint32_t) stat.size; /* total size, in bytes */
#ifndef __APPLE__
			out->st_atim = {time(nullptr)}; /* time of last access */
			out->st_mtim = {time(nullptr)}; /* time of last modification */
			out->st_ctim = {time(nullptr)}; /* time of last change */
#endif
			return 0;
		} else {
			debug("FUSE: getattr: %s: not found", path);
			return 1;
		}
	};

	ops.mkdir = [](const char *path, mode_t) -> int {
		debug("FUSE: mkdir: %s", path);
		return fs()->mkdir(path);
	};

	ops.open = [](const char *path, struct fuse_file_info*) -> int {
		debug("FUSE: open: %s", path);
		return 0;
	};

	ops.read = [](const char *path, char *buffer, size_t size,
	              off_t offset, struct fuse_file_info *fi) -> int {
		debug("FUSE: read: %s", path);
		auto inode = fs()->stat(path).inode;
		size_t readSize = -1;
		fs()->read(inode, offset, size, buffer, &readSize);
		return readSize;
	};

	ops.readdir = [](const char *path, void *buffer, fuse_fill_dir_t filler,
	                 off_t offset, struct fuse_file_info *fi) -> int {
		debug("FUSE: readdir: %s", path);
		vector<DirectoryListing<string>> files;
		if (fs()->ls(path, &files) == 0) {
			for (auto file : files) {
				filler(buffer, file.name.c_str(), nullptr, 0);
			}
		}

		return 0;
	};

	ops.rename = [](const char *src, const char *dest) -> int {
		debug("FUSE: rename: %s to %s", src, dest);
		return fs()->move(src, dest);
	};

	ops.rmdir = [](const char *path) -> int {
		debug("FUSE: rmdir: %s", path);
		return fs()->remove(path);
	};

	ops.unlink = [](const char *path) -> int {
		debug("FUSE: unlink: %s", path);
		return fs()->remove(path);
	};

	return ops;
};

int mount(int argc, char **args) {
	logFile("/home/gary/log");
	int err = 0;
	size_t fsSize;
	auto fsPath = args[2];
	auto mountPoint = args[3];
	auto fsBuff = loadFileBuff(fsPath, &fsSize);
	if (fsBuff) {
		fuse = createFileSystem(fsBuff, fsSize);
		if (fuse) {
			const char *fuseArgs[2] = {(char*) "oxfuse", mountPoint};
			err = fuse_main(2, (char**) fuseArgs, &fuseOps(), nullptr);
		} else {
			err = 1;
		}
	} else {
		err = 2;
	}
	return err;
}
