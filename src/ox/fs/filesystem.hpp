/*
 * Copyright 2015 - 2017 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#pragma once

#include <ox/std/std.hpp>
#include "filestore.hpp"

namespace ox {
namespace fs {

enum FsType {
	OxFS_16 = 1,
	OxFS_32 = 2,
	OxFS_64 = 3
};

enum FileType {
	NormalFile = 1,
	Directory  = 2
};

struct FileStat {
	uint64_t inode;
	uint64_t size;
	uint8_t  fileType;
};

class FileSystem {
	public:
		virtual ~FileSystem() {};

		virtual int read(uint64_t inode, void *buffer, size_t size) = 0;

		virtual uint8_t *read(uint64_t inode, size_t *size) = 0;

		virtual int remove(uint64_t inode) = 0;

		virtual void resize(uint64_t size = 0) = 0;

		virtual int write(uint64_t inode, void *buffer, uint64_t size, uint8_t fileType = NormalFile) = 0;

		virtual FileStat stat(uint64_t inode) = 0;

		virtual uint64_t spaceNeeded(uint64_t id, uint64_t size) = 0;

		virtual uint64_t available() = 0;

		virtual uint64_t size() = 0;
};

FileSystem *createFileSystem(void *buff);

template<typename FileStore, FsType FS_TYPE>
class FileSystemTemplate: public FileSystem {

	private:
		struct DirectoryEntry {
			typename FileStore::InodeId_t inode;

			char *getName() {
				return (char*) (this + 1);
			}

			void setName(const char *name) {
				auto data = getName();
				auto nameLen = ox_strlen(name);
				ox_memcpy(data, &name, nameLen);
				data[nameLen] = 0;
			}
		};

		struct Directory {
			/**
			 * Number of files in this directory.
			 */
			typename FileStore::InodeId_t size = 0;

			DirectoryEntry *files() {
				return (DirectoryEntry*) (this + 1);
			}
		};

		// static members
		static typename FileStore::InodeId_t INODE_ROOT_DIR;

		FileStore *store = nullptr;

	public:
		FileSystemTemplate(void *buff);

		int mkdir(const char *path);

		int read(const char *path, void *buffer);

		uint8_t *read(uint64_t inode, size_t *size) override;

		int read(uint64_t inode, void *buffer, size_t size) override;

		void resize(uint64_t size = 0) override;

		int remove(uint64_t inode) override;

		int write(uint64_t inode, void *buffer, uint64_t size, uint8_t fileType) override;

		FileStat stat(const char *path);

		FileStat stat(uint64_t inode) override;

		uint64_t spaceNeeded(uint64_t id, uint64_t size) override;

		uint64_t available() override;

		uint64_t size() override;

		static uint8_t *format(void *buffer, typename FileStore::FsSize_t size, bool useDirectories);
};

template<typename FileStore, FsType FS_TYPE>
FileSystemTemplate<FileStore, FS_TYPE>::FileSystemTemplate(void *buff) {
	store = (FileStore*) buff;
}

template<typename FileStore, FsType FS_TYPE>
typename FileStore::InodeId_t FileSystemTemplate<FileStore, FS_TYPE>::INODE_ROOT_DIR = 2;

template<typename FileStore, FsType FS_TYPE>
int FileSystemTemplate<FileStore, FS_TYPE>::mkdir(const char *path) {
	return 0;
}

template<typename FileStore, FsType FS_TYPE>
FileStat FileSystemTemplate<FileStore, FS_TYPE>::stat(const char *path) {
	FileStat stat;
	return stat;
}

#ifdef _MSC_VER
#pragma warning(disable:4244)
#endif
template<typename FileStore, FsType FS_TYPE>
FileStat FileSystemTemplate<FileStore, FS_TYPE>::stat(uint64_t inode) {
	FileStat stat;
	auto s = store->stat(inode);
	stat.size = s.size;
	stat.inode = s.inodeId;
	stat.fileType = s.fileType;
	return stat;
}
#ifdef _MSC_VER
#pragma warning(default:4244)
#endif

#ifdef _MSC_VER
#pragma warning(disable:4244)
#endif
template<typename FileStore, FsType FS_TYPE>
int FileSystemTemplate<FileStore, FS_TYPE>::read(uint64_t inode, void *buffer, size_t size) {
	auto err = 1;
	auto s = store->stat(inode);
	if (size == s.size) {
		err = store->read(inode, buffer, nullptr);
	}
	return err;
}
#ifdef _MSC_VER
#pragma warning(default:4244)
#endif

#ifdef _MSC_VER
#pragma warning(disable:4244)
#endif
template<typename FileStore, FsType FS_TYPE>
uint8_t *FileSystemTemplate<FileStore, FS_TYPE>::read(uint64_t inode, size_t *size) {
	auto s = store->stat(inode);
	auto buff = new uint8_t[s.size];
	if (size) {
		*size = s.size;
	}
	if (store->read(inode, buff, nullptr)) {
		delete []buff;
		buff = nullptr;
	}
	return buff;
}
#ifdef _MSC_VER
#pragma warning(default:4244)
#endif

#ifdef _MSC_VER
#pragma warning(disable:4244)
#endif
template<typename FileStore, FsType FS_TYPE>
int FileSystemTemplate<FileStore, FS_TYPE>::remove(uint64_t inode) {
	return store->remove(inode);
}
#ifdef _MSC_VER
#pragma warning(default:4244)
#endif

#ifdef _MSC_VER
#pragma warning(disable:4244)
#endif
template<typename FileStore, FsType FS_TYPE>
int FileSystemTemplate<FileStore, FS_TYPE>::write(uint64_t inode, void *buffer, uint64_t size, uint8_t fileType) {
	return store->write(inode, buffer, size, fileType);
}
#ifdef _MSC_VER
#pragma warning(default:4244)
#endif

template<typename FileStore, FsType FS_TYPE>
void FileSystemTemplate<FileStore, FS_TYPE>::resize(uint64_t size) {
	return store->resize(size);
}

template<typename FileStore, FsType FS_TYPE>
uint64_t FileSystemTemplate<FileStore, FS_TYPE>::spaceNeeded(uint64_t id, uint64_t size) {
	return store->spaceNeeded(id, size);
}

template<typename FileStore, FsType FS_TYPE>
uint64_t FileSystemTemplate<FileStore, FS_TYPE>::available() {
	return store->available();
}

template<typename FileStore, FsType FS_TYPE>
uint64_t FileSystemTemplate<FileStore, FS_TYPE>::size() {
	return store->size();
}

#ifdef _MSC_VER
#pragma warning(disable:4244)
#endif
template<typename FileStore, FsType FS_TYPE>
uint8_t *FileSystemTemplate<FileStore, FS_TYPE>::format(void *buffer, typename FileStore::FsSize_t size, bool useDirectories) {
	buffer = FileStore::format((uint8_t*) buffer, size, (uint16_t) FS_TYPE);
	FileSystemTemplate<FileStore, FS_TYPE> fs(buffer);

	if (buffer && useDirectories) {
		char dirBuff[sizeof(Directory) + sizeof(DirectoryEntry) + 2];
		auto *dir = (Directory*) dirBuff;
		dir->files();
		fs.write(INODE_ROOT_DIR, dirBuff, useDirectories, FileType::Directory);
	}

	return (uint8_t*) buffer;
}
#ifdef _MSC_VER
#pragma warning(default:4244)
#endif

typedef FileSystemTemplate<FileStore16, OxFS_16> FileSystem16;
typedef FileSystemTemplate<FileStore32, OxFS_32> FileSystem32;
typedef FileSystemTemplate<FileStore64, OxFS_64> FileSystem64;

}
}
