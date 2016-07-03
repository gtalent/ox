/*
 * Copyright 2015 - 2016 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include <stdio.h>
#include <stdlib.h>
#include <ox/std/strops.hpp>
#include <ox/fs/filesystem.hpp>

using namespace ox::fs;

const char *usage = "usage:\n"
"\toxfs format [16,32,64] <size> <path>\n"
"\toxfs write <FS file> <inode> <insertion file>";

char *loadFileBuff(const char *path, ::size_t *sizeOut = nullptr) {
	FILE *file = fopen(path, "rw");
	fseek(file, 0, SEEK_END);
	const auto size = ftell(file);
	auto buff = (char*) malloc(size);
	fread(buff, size, 1, file);
	fclose(file);
	if (sizeOut) {
		*sizeOut = size;
	}
	return buff;
}

int format(int argc, char **args) {
	printf("Creating file system...\n");
	auto err = 1;
	if (argc >= 5) {
		auto type = ox::std::atoi(args[2]);
		auto size = ox::std::atoi(args[3]);
		auto path = args[4];
		auto buff = (ox::std::uint8_t*) malloc(size);
		
		printf("Size: %d bytes\n", size);
		printf("Type: %d\n", type);

		// format
		switch (type) {
			case 16:
				FileStore16::format(buff, size, ox::fs::OxFS16);
				break;
			case 32:
				FileStore32::format(buff, size, ox::fs::OxFS32);
				break;
			case 64:
				FileStore64::format(buff, size, ox::fs::OxFS64);
				break;
		}

		FILE *file = fopen(path, "w");
		if (file) {
			fwrite(buff, size, 1, file);
			err = fclose(file);
			if (err) {
				printf("Could not write to file: %s.\n", path);
			}
		} else {
			printf("Could not open file: %s.\n", path);
		}

		if (err == 0) {
			printf("Created file system %s\n", path);
		}
	}

	return err;
}

int write(int argc, char **args) {
	auto err = 0;
	if (argc >= 5) {
		auto fsPath = args[2];
		auto inode = ox::std::atoi(args[3]);
		auto srcPath = args[4];
		::size_t srcSize;

		FILE *fsFile = fopen(fsPath, "rw");
		fseek(fsFile, 0, SEEK_END);

		const auto fsSize = ftell(fsFile);
		auto fs = (char*) malloc(fsSize);
		fread(fs, fsSize, 1, fsFile);

		auto srcBuff = loadFileBuff(srcPath, &srcSize);

		auto type = *((ox::std::uint32_t*) fs);
		switch (type) {
			case ox::fs::OxFS16:
				((FileStore16*) fs)->write(inode, srcBuff, srcSize);
				break;
			case ox::fs::OxFS32:
				((FileStore32*) fs)->write(inode, srcBuff, srcSize);
				break;
			case ox::fs::OxFS64:
				((FileStore64*) fs)->write(inode, srcBuff, srcSize);
				break;
		}

		fwrite(fs, fsSize, 1, fsFile);
		err = fclose(fsFile);
	}
	return err;
}

int main(int argc, char **args) {
	auto err = 0;
	if (argc > 1) {
		auto cmd = args[1];
		if (::strcmp(cmd, "format") == 0) {
			err = format(argc, args);
		} else if (::strcmp(cmd, "write") == 0) {
			err = write(argc, args);
		} else if (::strcmp(cmd, "help") == 0) {
			printf("%s\n", usage);
		} else {
			printf("Command '%s' not recognized.\n", cmd);
			err = 1;
		}
	} else {
		printf("%s\n", usage);
	}
	return err;
}
