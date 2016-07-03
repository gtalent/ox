/*
 * Copyright 2015 - 2016 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ox/std/strops.hpp>
#include <ox/fs/filesystem.hpp>

using namespace ox::fs;

const char *usage = "usage options:\n"
"\toxfs format [16,32,64] <size> <path>";

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

int main(int argc, char **args) {
	auto err = 0;
	if (argc > 1) {
		auto cmd = args[1];
		if (::strcmp(cmd, "format") == 0) {
			err = format(argc, args);
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
