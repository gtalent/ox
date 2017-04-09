/*
 * Copyright 2015 - 2017 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include <iostream>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ox/std/strops.hpp>
#include <ox/fs/filesystem.hpp>

// suppress warnings about using fopen
#ifdef _MSC_VER
#pragma warning(disable:4996)
#endif

using namespace ox::fs;
using namespace std;

const static auto oxfstoolVersion = "1.1.1";
const static auto usage = "usage:\n"
"\toxfs format [16,32,64] <size> <path>\n"
"\toxfs read <FS file> <inode>\n"
"\toxfs write <FS file> <inode> <insertion file>\n"
"\toxfs write-expand <FS file> <inode> <insertion file>\n"
"\toxfs rm <FS file> <inode>\n"
"\toxfs compact <FS file>\n"
"\toxfs version\n";

char *loadFileBuff(const char *path, ::size_t *sizeOut = nullptr) {
	auto file = fopen(path, "rb");
	if (file) {
		fseek(file, 0, SEEK_END);
		const auto size = ftell(file);
		rewind(file);
		auto buff = (char*) malloc(size);
		auto itemsRead = fread(buff, size, 1, file);
		fclose(file);
		if (sizeOut) {
			*sizeOut = itemsRead ? size : 0;
		}
		return buff;
	} else {
		return nullptr;
	}
}

size_t bytes(const char *str) {
	auto size = ::ox_strlen(str);
	const auto lastChar = str[size-1];
	auto multiplier = 1;
	auto copy = new char[size];
	ox_memcpy(copy, str, size);
	if (lastChar < '0' || lastChar > '9') {
		copy[size-1] = 0;
		switch (lastChar) {
			case 'k':
			case 'K':
				multiplier = 1024;
				break;
			case 'm':
			case 'M':
				multiplier = 1024 * 1024;
				break;
			case 'g':
			case 'G':
				multiplier = 1024 * 1024 * 1024;
				break;
			default:
				multiplier = -1;
		}
	}
	const auto retval = ((size_t) ox_atoi(copy)) * multiplier;
	delete []copy;
	return  retval;
}

int format(int argc, char **args) {
	printf("Creating file system...\n");
	auto err = 0;
	if (argc >= 5) {
		auto type = ox_atoi(args[2]);
		cout << args[3] << endl;
		auto size = bytes(args[3]);
		auto path = args[4];
		auto buff = (uint8_t*) malloc(size);

		cout << "Size: " << size << " bytes\n";
		cout << "Type: " << type << endl;

		if (size < sizeof(FileStore64)) {
			err = 1;
			cerr <<  "File system size " << size << " too small, must be at least " << sizeof(FileStore64) << endl;
		}

		if (!err) {
			// format
			switch (type) {
				case 16:
					FileStore16::format(buff, (FileStore16::FsSize_t) size, ox::fs::OxFS_16);
					break;
				case 32:
					FileStore32::format(buff, (FileStore32::FsSize_t) size, ox::fs::OxFS_32);
					break;
				case 64:
					FileStore64::format(buff, size, ox::fs::OxFS_64);
					break;
				default:
					err = 1;
			}

			if (!err) {
				auto file = fopen(path, "wb");
				if (file) {
					err = fwrite(buff, size, 1, file) != 1;
					err |= fclose(file);
					if (err) {
						fprintf(stderr, "Could not write to file: %s.\n", path);
					}
				} else {
					fprintf(stderr, "Could not open file: %s.\n", path);
				}
			}
		}

		free(buff);

		if (err == 0) {
			fprintf(stderr, "Created file system %s\n", path);
		}
	} else {
		fprintf(stderr, "Insufficient arguments\n");
	}

	return err;
}

int read(int argc, char **args) {
	auto err = 1;
	if (argc >= 4) {
		auto fsPath = args[2];
		auto inode = ox_atoi(args[3]);
		size_t fsSize;
		size_t fileSize;

		auto fsBuff = loadFileBuff(fsPath, &fsSize);
		
		if (fsBuff) {
			auto fs = createFileSystem(fsBuff);

			if (fs) {
				auto output = fs->read(inode, &fileSize);

				if (output) {
					fwrite(output, fileSize, 1, stdout);
					delete []output;
					err = 0;
				}

				delete fs;
				free(fsBuff);
			} else {
				fprintf(stderr, "Invalid file system type: %d.\n", *(uint32_t*) fsBuff);
			}
		} else {
			fprintf(stderr, "Could not open file: %s\n", fsPath);
		}
	} else {
		fprintf(stderr, "Insufficient arguments\n");
	}
	return err;
}

int write(int argc, char **args, bool expand) {
	auto err = 0;
	if (argc >= 5) {
		auto fsPath = args[2];
		auto inode = ox_atoi(args[3]);
		auto srcPath = args[4];
		size_t srcSize;

		auto fsFile = fopen(fsPath, "rb");
		if (fsFile) {
			fseek(fsFile, 0, SEEK_END);

			auto fsSize = (size_t) ftell(fsFile);
			rewind(fsFile);
			auto fsBuff = new uint8_t[fsSize];
			auto itemsRead = fread(fsBuff, fsSize, 1, fsFile);
			fclose(fsFile);

			if (itemsRead) {
				auto srcBuff = loadFileBuff(srcPath, &srcSize);
				if (srcBuff) {
					auto fs = createFileSystem(fsBuff);
					if (fs) {
						if (expand && fs->available() <= srcSize) {
							auto needed = fs->spaceNeeded(inode, srcSize);
							auto cloneBuff = new uint8_t[needed];
							ox_memcpy(cloneBuff, fsBuff, fsSize);

							delete fs;
							delete []fsBuff;

							fsBuff = cloneBuff;
							fs = createFileSystem(fsBuff);
							fsSize = needed;
							fs->resize(fsSize);
						}
						err |= fs->write(inode, srcBuff, srcSize);
						if (err) {
							fprintf(stderr, "Could not write to file system.\n");
						}
						delete fs;
					} else {
						fprintf(stderr, "Invalid file system type: %d.\n", *(uint32_t*) fsBuff);
						err = 1;
					}

					if (!err) {
						fsFile = fopen(fsPath, "wb");

						if (fsFile) {
							err = fwrite(fsBuff, fsSize, 1, fsFile) != 1;
							err |= fclose(fsFile);
							if (err) {
								fprintf(stderr, "Could not write to file system file.\n");
							}
						} else {
							err = 1;
						}
					}
					free(srcBuff);
				} else {
					err = 1;
					fprintf(stderr, "Could not load source file: %s.\n", srcPath);
				}
			}

			delete []fsBuff;
		} else {
			fprintf(stderr, "Could not open file system\n");
		}
	} else {
		fprintf(stderr, "Insufficient arguments\n");
	}
	return err;
}

int compact(int argc, char **args) {
	auto err = 1;
	if (argc >= 2) {
		auto fsPath = args[2];
		size_t fsSize;

		auto fsBuff = loadFileBuff(fsPath, &fsSize);
		if (fsBuff) {
			auto fs = createFileSystem(fsBuff);

			if (fs) {
				fs->resize();
			} else {
				fprintf(stderr, "Invalid file system.\n");
			}

			// write back to file
			auto fsFile = fopen(fsPath, "wb");
			if (fsFile) {
				err = fwrite(fsBuff, fs->size(), 1, fsFile) != 1;
				err |= fclose(fsFile);
				if (err) {
					fprintf(stderr, "Could not write to file system file.\n");
				}
			} else {
				err = 1;
			}

			delete fs;
			free(fsBuff);
		} else {
			fprintf(stderr, "Could not open file: %s\n", fsPath);
		}
	} else {
		fprintf(stderr, "Insufficient arguments\n");
	}
	return err;
}

int remove(int argc, char **args) {
	auto err = 1;
	if (argc >= 4) {
		auto fsPath = args[2];
		auto inode = ox_atoi(args[3]);
		size_t fsSize;

		auto fsBuff = loadFileBuff(fsPath, &fsSize);
		if (fsBuff) {
			auto fs = createFileSystem(fsBuff);

			if (fs) {
				err = fs->remove(inode);
			} else {
				fprintf(stderr, "Invalid file system.\n");
			}

			if (err) {
				fprintf(stderr, "Could not write to file system.\n");
			} else {
				auto fsFile = fopen(fsPath, "wb");
				if (fsFile) {
					err = fwrite(fsBuff, fsSize, 1, fsFile) != 1;
					err |= fclose(fsFile);
					if (err) {
						fprintf(stderr, "Could not write to file system file.\n");
					}
				} else {
					err = 1;
				}
			}

			delete fs;
			free(fsBuff);
		} else {
			fprintf(stderr, "Could not open file: %s\n", fsPath);
		}
	} else {
		fprintf(stderr, "Insufficient arguments\n");
	}
	return err;
}

int main(int argc, char **args) {
	auto err = 0;
	if (argc > 1) {
		auto cmd = args[1];
		if (ox_strcmp(cmd, "format") == 0) {
			err = format(argc, args);
		} else if (ox_strcmp(cmd, "read") == 0) {
			err = read(argc, args);
		} else if (ox_strcmp(cmd, "write") == 0) {
			err = write(argc, args, false);
		} else if (ox_strcmp(cmd, "write-expand") == 0) {
			err = write(argc, args, true);
		} else if (ox_strcmp(cmd, "compact") == 0) {
			err = compact(argc, args);
		} else if (ox_strcmp(cmd, "rm") == 0) {
			err = remove(argc, args);
		} else if (ox_strcmp(cmd, "help") == 0) {
			printf("%s\n", usage);
		} else if (ox_strcmp(cmd, "version") == 0) {
			printf("oxfstool version %s\n", oxfstoolVersion);
			printf("oxfs format version %d\n", FileStore16::VERSION);
		} else {
			printf("Command '%s' not recognized.\n", cmd);
			err = 1;
		}
	} else {
		printf("%s\n", usage);
	}
	return err;
}
