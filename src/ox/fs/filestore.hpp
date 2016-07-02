/*
 * Copyright 2015 - 2016 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#pragma once

#include <ox/std/std.hpp>

namespace ox {
namespace fs {

using namespace ox::std;

template<typename FsT>
class FileStore {

	public:
		typedef uint16_t InodeId_t;
		typedef FsT FsSize_t;

		struct StatInfo {
			InodeId_t inodeId;
			FsSize_t  size;
		};

	private:
		struct Inode {
			// the next Inode in memory
			FsSize_t prev, next;

			// The following variables should not be assumed to exist
			FsSize_t dataLen;
			InodeId_t m_id;
			uint8_t refs;
			FsSize_t left, right;

			FsSize_t size();
			void setId(InodeId_t);
			void setData(void *data, int size);
			void *data();

			private:
				Inode() = default;
		};

		uint32_t m_fsType;
		FsSize_t m_size;
		FsSize_t m_firstInode;
		FsSize_t m_rootInode;

	public:
		/**
		 * Initializes the memory chunk of this FileStore was given.
		 * This clears the previous contents.
		 */
		void init();

		/**
		 * Writes the given data to a "file" with the given id.
		 * @param id the id of the file
		 * @param data the contents of the file
		 * @param dataLen the number of bytes data points to
		 */
		int write(void *data, FsSize_t dataLen);

		/**
		 * Writes the given data to a "file" with the given id.
		 * @param id the id of the file
		 * @param data the contents of the file
		 * @param dataLen the number of bytes data points to
		 */
		int write(InodeId_t id, void *data, FsSize_t dataLen);

		/**
		 * Reads the "file" at the given id. You are responsible for freeing
		 * the data when done with it.
		 * @param id id of the "file"
		 * @param data pointer to the pointer where the data is stored
		 * @param size pointer to a value that will be assigned the size of data
		 * @return 0 if read is a success
		 */
		int read(InodeId_t id, void *data, FsSize_t *size);

		/**
		 * Reads the stat information of the inode of the given inode id.
		 * If the returned inode id is 0, then the requested inode was not found.
		 * @param id id of the inode to stat
		 * @return the stat information of the inode of the given inode id
		 */
		StatInfo stat(InodeId_t id);

		static uint8_t version();

		static uint8_t *format(uint8_t *buffer, FsSize_t size, uint32_t fsType = 0);

	private:
		/**
		 * Gets the inode at the given id.
		 * @param root the root node to start comparing on
		 * @param id id of the "file"
		 * @param pathLen number of characters in pathLen
		 * @return the requested Inode, if available
		 */
		Inode *getInode(Inode *root, InodeId_t id);

		/**
		 * Gets an address for a new Inode.
		 * @param size the size of the Inode
		 */
		void *alloc(FsSize_t size);

		/**
		 * Compresses all of the inode into a contiguous space, starting at m_firstInode.
		 */
		void compress();

		/**
		 * Inserts the given insertValue into the tree of the given root.
		 * If the inode already exists, it replaces the old on deletes it.
		 * @return true if the inode was inserted
		 */
		bool insert(Inode *root, Inode *insertValue);

		/**
		 * Gets the FsSize_t associated with the next Inode to be allocated.
		 * @retrun the FsSize_t associated with the next Inode to be allocated
		 */
		FsSize_t iterator();

		Inode *firstInode();

		Inode *lastInode();

		uint8_t *begin() {
			return (uint8_t*) this;
		}

		uint8_t *end() {
			return begin() + this->m_size;
		}

		/**
		 * Converts an actual pointer to a FsSize_t.
		 */
		FsSize_t ptr(void *ptr);

		/**
		 * Converts a FsSize_t to an actual pointer.
		 */
		template<typename T>
		T ptr(FsSize_t ptr) {
			return (T) (begin() + ptr);
		};

};

template<typename FsSize_t>
FsSize_t FileStore<FsSize_t>::Inode::size() {
	return sizeof(Inode) + dataLen;
}

template<typename FsSize_t>
void FileStore<FsSize_t>::Inode::setId(InodeId_t id) {
	this->m_id = id;
}

template<typename FsSize_t>
void FileStore<FsSize_t>::Inode::setData(void *data, int size) {
	memcpy(this->data(), data, size);
	dataLen = size;
}


template<typename FsSize_t>
void *FileStore<FsSize_t>::Inode::data() {
	return this + 1;
}


// FileStore

template<typename FsSize_t>
int FileStore<FsSize_t>::write(void *data, FsSize_t dataLen) {
	return 1;
}

template<typename FsSize_t>
int FileStore<FsSize_t>::write(InodeId_t id, void *data, FsSize_t dataLen) {
	auto retval = 1;
	const FsSize_t size = sizeof(Inode) + dataLen;
	//printf("%d\n", m_rootInode);
	auto inode = (Inode*) alloc(size);
	if (inode) {
		auto root = ptr<Inode*>(m_rootInode);
		inode->m_id = id;
		inode->setData(data, dataLen);
		if (insert(root, inode) || root == inode) {
			retval = 0;
		}
	}
	return retval;
}

template<typename FsSize_t>
int FileStore<FsSize_t>::read(InodeId_t id, void *data, FsSize_t *size) {
	auto inode = getInode(ptr<Inode*>(m_rootInode), id);
	int retval = 1;
	if (inode) {
		if (size) {
			*size = inode->dataLen;
		}
		memcpy(data, inode->data(), inode->dataLen);
		retval = 0;
	}
	return retval;
}

template<typename FsSize_t>
typename FileStore<FsSize_t>::StatInfo FileStore<FsSize_t>::stat(InodeId_t id) {
	auto inode = getInode(ptr<Inode*>(m_rootInode), id);
	StatInfo stat;
	if (inode) {
		stat.size = inode->dataLen;
		stat.inodeId = id;
	} else {
		stat.inodeId = 0;
	}
	return stat;
}

template<typename FsSize_t>
typename FileStore<FsSize_t>::Inode *FileStore<FsSize_t>::getInode(Inode *root, InodeId_t id) {
	Inode *retval = nullptr;

	if (root->m_id > id) {
		if (root->left) {
			retval = getInode(ptr<Inode*>(root->left), id);
		}
	} else if (root->m_id < id) {
		if (root->right) {
			retval = getInode(ptr<Inode*>(root->right), id);
		}
	} else if (root->m_id == id) {
		retval = root;
	}

	return retval;
}

template<typename FsSize_t>
void *FileStore<FsSize_t>::alloc(FsSize_t size) {
	if ((lastInode()->next + size) > (uint64_t) end()) {
		compress();
		if ((lastInode()->next + size) > (uint64_t) end()) {
			return nullptr;
		}
	}

	const auto retval = lastInode()->next;
	const auto inode = ptr<Inode*>(retval);
	memset(inode, 0, size);
	inode->next = retval + size;
	ptr<Inode*>(m_firstInode)->prev = retval;
	return inode;
}

template<typename FsSize_t>
void FileStore<FsSize_t>::compress() {
	auto current = ptr<Inode*>(m_firstInode);
	while (current->next) {
		auto prevEnd = current + current->size();
		current = ptr<Inode*>(current->next);
		if (prevEnd != current) {
			memcpy(prevEnd, current, current->size());
			current = prevEnd;
		}
	}
}

template<typename FsSize_t>
bool FileStore<FsSize_t>::insert(Inode *root, Inode *insertValue) {
	auto retval = false;

	if (root->m_id > insertValue->m_id) {
		if (root->left) {
			retval = insert(ptr<Inode*>(root->left), insertValue);
		} else {
			root->left = ptr(insertValue);
			retval = true;
		}
	} else if (root->m_id < insertValue->m_id) {
		if (root->right) {
			retval = insert(ptr<Inode*>(root->right), insertValue);
		} else {
			root->right = ptr(insertValue);
			retval = true;
		}
	}

	return retval;
}

template<typename FsSize_t>
FsSize_t FileStore<FsSize_t>::iterator() {
	return ptr(lastInode()) + lastInode()->size();
}

template<typename FsSize_t>
FsSize_t FileStore<FsSize_t>::ptr(void *ptr) {
	return ((uint8_t*) ptr) - begin();
}

template<typename FsSize_t>
typename FileStore<FsSize_t>::Inode *FileStore<FsSize_t>::firstInode() {
	return ptr<Inode*>(sizeof(FileStore<FsSize_t>));
}

template<typename FsSize_t>
typename FileStore<FsSize_t>::Inode *FileStore<FsSize_t>::lastInode() {
	return ptr<Inode*>(ptr<Inode*>(m_firstInode)->prev);
}

template<typename FsSize_t>
uint8_t FileStore<FsSize_t>::version() {
	return 0;
};

template<typename FsSize_t>
uint8_t *FileStore<FsSize_t>::format(uint8_t *buffer, FsSize_t size, uint32_t fsType) {
	memset(buffer, 0, size);

	auto *fs = (FileStore*) buffer;
	fs->m_fsType = fsType;
	fs->m_size = size;
	fs->m_rootInode = sizeof(FileStore<FsSize_t>);
	fs->m_firstInode = sizeof(FileStore<FsSize_t>);
	fs->firstInode()->prev = fs->m_firstInode;
	fs->lastInode()->next = sizeof(FileStore<FsSize_t>);

	return (uint8_t*) buffer;
}

typedef FileStore<uint16_t> FileStore16;
typedef FileStore<uint32_t> FileStore32;
typedef FileStore<uint64_t> FileStore64;

}
}
