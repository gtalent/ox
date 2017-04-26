/*
 * Copyright 2015 - 2017 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#pragma once

#include <ox/std/std.hpp>
#include "pathiterator.hpp"
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

		virtual int mkdir(const char *path) = 0;

		virtual int read(const char *path, void *buffer, size_t buffSize) = 0;

		virtual int read(uint64_t inode, void *buffer, size_t size) = 0;

		virtual int read(uint64_t inode, size_t readStart, size_t readSize, void *buffer, size_t *size) = 0;

		virtual uint8_t *read(uint64_t inode, size_t *size) = 0;

		virtual int remove(uint64_t inode) = 0;

		virtual void resize(uint64_t size = 0) = 0;

		virtual int write(const char *path, void *buffer, uint64_t size, uint8_t fileType = NormalFile) = 0;

		virtual int write(uint64_t inode, void *buffer, uint64_t size, uint8_t fileType = NormalFile) = 0;

		virtual FileStat stat(uint64_t inode) = 0;

		virtual uint64_t spaceNeeded(uint64_t size) = 0;

		virtual uint64_t available() = 0;

		virtual uint64_t size() = 0;

		virtual uint8_t *buff() = 0;
};

FileSystem *createFileSystem(void *buff, size_t buffSize);

/**
 * Creates a larger version of the given FileSystem.
 */
FileSystem *expandCopy(FileSystem *src);

/**
 * Calls expandCopy and deletes the original FileSystem and buff a resize was
 * performed.
 */
FileSystem *expandCopyCleanup(FileSystem *fs, size_t size);

template<typename FileStore, FsType FS_TYPE>
class FileSystemTemplate: public FileSystem {

	private:
		struct __attribute__((packed)) DirectoryEntry {
			typename FileStore::InodeId_t inode;

			char *getName() {
				return (char*) (this + 1);
			}

			void setName(const char *name) {
				auto data = getName();
				auto nameLen = ox_strlen(name);
				ox_memcpy(data, name, nameLen);
				data[nameLen] = 0;
			}

			/**
			 * The size in bytes.
			 */
			uint64_t size() {
				return sizeof(DirectoryEntry) + ox_strlen(getName());
			}

			static uint64_t spaceNeeded(const char *fileName) {
				return sizeof(DirectoryEntry) + ox_strlen(fileName) + 1;
			}
		};

		struct __attribute__((packed)) Directory {
			/**
			 * Number of bytes after this Directory struct.
			 */
			typename FileStore::FsSize_t size = 0;

			DirectoryEntry *files() {
				return size ? (DirectoryEntry*) (this + 1) : nullptr;
			}

			uint64_t getFileInode(const char *name, uint64_t buffSize);
		};

		FileStore *m_store = nullptr;

	public:
		// static members
		static typename FileStore::InodeId_t INODE_ROOT_DIR;

		explicit FileSystemTemplate(void *buff);

		int mkdir(const char *path) override;

		int read(const char *path, void *buffer, size_t buffSize) override;

		int read(uint64_t inode, void *buffer, size_t buffSize) override;

		int read(uint64_t inode, size_t readStart, size_t readSize, void *buffer, size_t *size) override;

		uint8_t *read(uint64_t inode, size_t *size) override;

		void resize(uint64_t size = 0) override;

		int remove(uint64_t inode) override;

		int write(const char *path, void *buffer, uint64_t size, uint8_t fileType = NormalFile) override;

		int write(uint64_t inode, void *buffer, uint64_t size, uint8_t fileType) override;

		FileStat stat(const char *path);

		FileStat stat(uint64_t inode) override;

		uint64_t findInodeOf(const char *name);

		uint64_t spaceNeeded(uint64_t size) override;

		uint64_t available() override;

		uint64_t size() override;

		uint8_t *buff() override;

		static uint8_t *format(void *buffer, typename FileStore::FsSize_t size, bool useDirectories);

	private:
		int insertDirectoryEntry(const char *dirPath, const char *fileName, uint64_t inode);
};

template<typename FileStore, FsType FS_TYPE>
FileSystemTemplate<FileStore, FS_TYPE>::FileSystemTemplate(void *buff) {
	m_store = (FileStore*) buff;
}

template<typename FileStore, FsType FS_TYPE>
typename FileStore::InodeId_t FileSystemTemplate<FileStore, FS_TYPE>::INODE_ROOT_DIR = 2;

template<typename FileStore, FsType FS_TYPE>
int FileSystemTemplate<FileStore, FS_TYPE>::mkdir(const char *path) {
	Directory dir;
	return write(path, &dir, sizeof(dir), FileType::Directory);
}

template<typename FileStore, FsType FS_TYPE>
FileStat FileSystemTemplate<FileStore, FS_TYPE>::stat(const char *path) {
	auto inode = findInodeOf(path);
	FileStat stat;
	auto s = m_store->stat(inode);
	stat.size = s.size;
	stat.inode = s.inodeId;
	stat.fileType = s.fileType;
	return stat;
}

#ifdef _MSC_VER
#pragma warning(disable:4244)
#endif
template<typename FileStore, FsType FS_TYPE>
FileStat FileSystemTemplate<FileStore, FS_TYPE>::stat(uint64_t inode) {
	FileStat stat;
	auto s = m_store->stat(inode);
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
int FileSystemTemplate<FileStore, FS_TYPE>::read(const char *path, void *buffer, size_t buffSize) {
	int retval = -1;

	// find the inode for the given path
	auto inode = findInodeOf(path);

	// if inode exists, read the data into buffer
	if (inode) {
		read(inode, buffer, buffSize);
	}

	return retval;
}
#ifdef _MSC_VER
#pragma warning(default:4244)
#endif

#ifdef _MSC_VER
#pragma warning(disable:4244)
#endif
template<typename FileStore, FsType FS_TYPE>
int FileSystemTemplate<FileStore, FS_TYPE>::read(uint64_t inode, void *buffer, size_t buffSize) {
	auto stat = m_store->stat(inode);
	if (stat.size <= buffSize) {
		return m_store->read(inode, buffer, nullptr);
	}
	return -1;
;
}
#ifdef _MSC_VER
#pragma warning(default:4244)
#endif

#ifdef _MSC_VER
#pragma warning(disable:4244)
#endif
template<typename FileStore, FsType FS_TYPE>
int FileSystemTemplate<FileStore, FS_TYPE>::read(uint64_t inode, size_t readStart,
                                                 size_t readSize, void *buffer,
                                                 size_t *size) {
	if (size) {
		auto stat = m_store->stat(inode);
		*size = stat.size;
	}
	return m_store->read(inode, readStart, readSize, buffer, nullptr);
}
#ifdef _MSC_VER
#pragma warning(disable:4244)
#endif

#ifdef _MSC_VER
#pragma warning(disable:4244)
#endif
template<typename FileStore, FsType FS_TYPE>
uint8_t *FileSystemTemplate<FileStore, FS_TYPE>::read(uint64_t inode, size_t *size) {
	auto s = m_store->stat(inode);
	auto buff = new uint8_t[s.size];
	if (size) {
		*size = s.size;
	}
	if (m_store->read(inode, buff, nullptr)) {
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
	return m_store->remove(inode);
}
#ifdef _MSC_VER
#pragma warning(default:4244)
#endif

#ifdef _MSC_VER
#pragma warning(disable:4244)
#endif
template<typename FileStore, FsType FS_TYPE>
int FileSystemTemplate<FileStore, FS_TYPE>::write(const char *path, void *buffer, uint64_t size, uint8_t fileType) {
	int err = 0;
	size_t pathLen = ox_strlen(path);
	char dirPath[pathLen];
	char fileName[pathLen];
	PathIterator pathReader(path, pathLen);
	err |= pathReader.fileName(fileName, pathLen);
	err |= pathReader.dirPath(dirPath, pathLen);
	if (err) {
		return err;
	}

	uint64_t inode = findInodeOf(path);
	// find an inode value for the given path
	if (!inode) {
		while (!inode) {
			inode = ox_rand() >> 48;
			// make sure this does not already exist
			if (stat(inode).inode) {
				// that result was unusable, try again
				inode = 0;
			}
		}
		err = insertDirectoryEntry(dirPath, fileName, inode);
	}

	if (!err) {
		err = write(inode, buffer, size, fileType);
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
int FileSystemTemplate<FileStore, FS_TYPE>::write(uint64_t inode, void *buffer, uint64_t size, uint8_t fileType) {
	return m_store->write(inode, buffer, size, fileType);
}
#ifdef _MSC_VER
#pragma warning(default:4244)
#endif

#ifdef _MSC_VER
#pragma warning(disable:4244)
#endif
template<typename FileStore, FsType FS_TYPE>
uint64_t FileSystemTemplate<FileStore, FS_TYPE>::findInodeOf(const char *path) {
	const auto pathLen = ox_strlen(path);
	PathIterator it(path, pathLen);
	char fileName[pathLen];
	uint64_t inode = INODE_ROOT_DIR;
	while (it.hasNext()) {
		auto dirStat = stat(inode);
		if (dirStat.inode && dirStat.size >= sizeof(Directory)) {
			uint8_t dirBuffer[dirStat.size];
			auto dir = (Directory*) dirBuffer;
			if (read(inode, dirBuffer, dirStat.size) == 0) {
				if (dirStat.fileType == FileType::Directory && it.next(fileName, pathLen) == 0) {
					inode = dir->getFileInode(fileName, dirStat.size);
				} else {
					inode = 0; // null out inode and break
					break;
				}
			} else {
				inode = 0; // null out inode and break
				break;
			}
		} else {
			inode = 0; // null out inode and break
			break;
		}
	}
	return inode;
}
#ifdef _MSC_VER
#pragma warning(default:4244)
#endif

template<typename FileStore, FsType FS_TYPE>
void FileSystemTemplate<FileStore, FS_TYPE>::resize(uint64_t size) {
	return m_store->resize(size);
}

template<typename FileStore, FsType FS_TYPE>
uint64_t FileSystemTemplate<FileStore, FS_TYPE>::spaceNeeded(uint64_t size) {
	return m_store->spaceNeeded(size);
}

template<typename FileStore, FsType FS_TYPE>
uint64_t FileSystemTemplate<FileStore, FS_TYPE>::available() {
	return m_store->available();
}

template<typename FileStore, FsType FS_TYPE>
uint64_t FileSystemTemplate<FileStore, FS_TYPE>::size() {
	return m_store->size();
}

template<typename FileStore, FsType FS_TYPE>
uint8_t *FileSystemTemplate<FileStore, FS_TYPE>::buff() {
	return (uint8_t*) m_store;
}

#ifdef _MSC_VER
#pragma warning(disable:4244)
#endif
template<typename FileStore, FsType FS_TYPE>
uint8_t *FileSystemTemplate<FileStore, FS_TYPE>::format(void *buffer, typename FileStore::FsSize_t size, bool useDirectories) {
	buffer = FileStore::format((uint8_t*) buffer, size, (uint16_t) FS_TYPE);

	if (buffer && useDirectories) {
		Directory dir;
		FileSystemTemplate<FileStore, FS_TYPE> fs(buffer);
		fs.write(INODE_ROOT_DIR, &dir, sizeof(dir), FileType::Directory);
	}

	return (uint8_t*) buffer;
}
#ifdef _MSC_VER
#pragma warning(default:4244)
#endif

#ifdef _MSC_VER
#pragma warning(disable:4244)
#endif
template<typename FileStore, FsType FS_TYPE>
int FileSystemTemplate<FileStore, FS_TYPE>::insertDirectoryEntry(const char *dirPath, const char *fileName, uint64_t inode) {
	auto s = stat(dirPath);
	if (s.inode) {
		size_t dirBuffSize = s.size + DirectoryEntry::spaceNeeded(fileName);
		uint8_t dirBuff[dirBuffSize];
		int err = read(s.inode, dirBuff, dirBuffSize);

		if (!err) {
			auto dir = (Directory*) dirBuff;
			dir->size++;
			auto entry = (DirectoryEntry*) &dirBuff[s.size];
			entry->inode = inode;
			entry->setName(fileName);
			return write(s.inode, dirBuff, dirBuffSize, FileType::Directory);
		} else {
			return 1;
		}
	} else {
		return 2;
	}
}
#ifdef _MSC_VER
#pragma warning(default:4244)
#endif


// Directory

template<typename FileStore, FsType FS_TYPE>
uint64_t FileSystemTemplate<FileStore, FS_TYPE>::Directory::getFileInode(const char *name, uint64_t buffSize) {
	uint64_t inode = 0;
	auto current = files();
	if (current) {
		for (uint64_t i = 1; ox_strcmp(current->getName(), name) != 0; i++) {
			if (i < this->size) {
				current = (DirectoryEntry*) (((uint8_t*) current) + current->size());
			} else {
				current = nullptr;
				break;
			}
		}
		if (current) {
			inode = current->inode;
		}
	}
	return inode;
}

typedef FileSystemTemplate<FileStore16, OxFS_16> FileSystem16;
typedef FileSystemTemplate<FileStore32, OxFS_32> FileSystem32;
typedef FileSystemTemplate<FileStore64, OxFS_64> FileSystem64;

}
}
