/*
 * Copyright 2015 - 2017 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <assert.h>
#include <map>
#include <string>
#include <ox/fs/filesystem.hpp>
#include <ox/fs/pathiterator.hpp>
#include <ox/std/std.hpp>

using namespace std;
using namespace ox::fs;

map<string, int(*)(string)> tests = {
	{
		{
			"PathIterator::next1",
			[](string) {
				int retval = 0;
				string path = "/usr/share/charset.gbag";
				PathIterator it(path.c_str(), path.size());
				const auto buffSize = 1024;
				char buff[buffSize];
				assert(buffSize >= path.size());
				retval |= !(it.next(buff, path.size()) == 0 && ox_strcmp(buff, "usr") == 0);
				retval |= !(it.next(buff, path.size()) == 0 && ox_strcmp(buff, "share") == 0);
				retval |= !(it.next(buff, path.size()) == 0 && ox_strcmp(buff, "charset.gbag") == 0);
				return retval;
			}
		},
		{
			"PathIterator::next2",
			[](string) {
				int retval = 0;
				string path = "/usr/share/";
				PathIterator it(path.c_str(), path.size());
				const auto buffSize = 1024;
				char buff[buffSize];
				assert(buffSize >= path.size());
				retval |= !(it.next(buff, path.size()) == 0 && ox_strcmp(buff, "usr") == 0);
				retval |= !(it.next(buff, path.size()) == 0 && ox_strcmp(buff, "share") == 0);
				return retval;
			}
		},
		{
			"PathIterator::next3",
			[](string) {
				int retval = 0;
				string path = "/";
				PathIterator it(path.c_str(), path.size());
				const auto buffSize = 1024;
				char buff[buffSize];
				assert(buffSize >= path.size());
				retval |= !(it.next(buff, path.size()) == 0 && ox_strcmp(buff, "\0") == 0);
				return retval;
			}
		},
		{
			"PathIterator::next4",
			[](string) {
				int retval = 0;
				string path = "usr/share/charset.gbag";
				PathIterator it(path.c_str(), path.size());
				const auto buffSize = 1024;
				char buff[buffSize];
				assert(buffSize >= path.size());
				retval |= !(it.next(buff, path.size()) == 0 && ox_strcmp(buff, "usr") == 0);
				retval |= !(it.next(buff, path.size()) == 0 && ox_strcmp(buff, "share") == 0);
				retval |= !(it.next(buff, path.size()) == 0 && ox_strcmp(buff, "charset.gbag") == 0);
				return retval;
			}
		},
		{
			"PathIterator::next5",
			[](string) {
				int retval = 0;
				string path = "usr/share/";
				PathIterator it(path.c_str(), path.size());
				const auto buffSize = 1024;
				char buff[buffSize];
				assert(buffSize >= path.size());
				retval |= !(it.next(buff, path.size()) == 0 && ox_strcmp(buff, "usr") == 0);
				retval |= !(it.next(buff, path.size()) == 0 && ox_strcmp(buff, "share") == 0);
				return retval;
			}
		},
		{
			"PathIterator::dirPath",
			[] (string) {
				int retval = 0;
				string path = "/usr/share/charset.gbag";
				PathIterator it(path.c_str(), path.size());
				const auto buffSize = 1024;
				char buff[buffSize];
				assert(buffSize >= path.size());
				retval |= !(it.dirPath(buff, path.size()) == 0 && ox_strcmp(buff, "/usr/share/") == 0);
				return retval;
			}
		},
		{
			"PathIterator::fileName",
			[](string) {
				int retval = 0;
				string path = "/usr/share/charset.gbag";
				PathIterator it(path.c_str(), path.size());
				const auto buffSize = 1024;
				char buff[buffSize];
				assert(buffSize >= path.size());
				retval |= !(it.fileName(buff, path.size()) == 0 && ox_strcmp(buff, "charset.gbag") == 0);
				return retval;
			}
		},
		{
			"FileSystem32::findInodeOf /",
			[](string) {
				int retval = 0;
				const auto size = 1024;
				uint8_t buff[size];
				FileSystem32::format(buff, (FileStore32::FsSize_t) size, true);
				auto fs = (FileSystem32*) createFileSystem(buff, size);
				retval |= !(fs->findInodeOf("/") == FileSystem32::INODE_ROOT_DIR);
				delete fs;
				return retval;
			}
		},
		{
			"FileSystem32::write(string)",
			[](string) {
				// this value will likely need to change if anything about the
				// random number generator changes
				const auto targetInode = 58542;

				int retval = 0;
				auto path = "/charset.gbag";
				auto data = "test";

				const auto size = 1024;
				uint8_t buff[size];
				FileSystem32::format(buff, (FileStore32::FsSize_t) size, true);
				auto fs = (FileSystem32*) createFileSystem(buff, size);

				retval |= fs->write(path, &data, ox_strlen(data));
				retval |= !(fs->findInodeOf(path) == targetInode);

				delete fs;
				return retval;
			}
		},
	},
};

int main(int argc, const char **args) {
	int retval = -1;
	if (argc > 1) {
		auto testName = args[1];
		string testArg = "";
		if (args[2]) {
			testArg = args[2];
		}
		if (tests.find(testName) != tests.end()) {
			retval = tests[testName](testArg);
		}
	}
	return retval;
}
