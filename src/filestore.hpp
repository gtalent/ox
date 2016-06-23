/*
 * Copyright 2015 - 2016 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef WOMBAT_FS_FILESTORE_HPP
#define WOMBAT_FS_FILESTORE_HPP

#include "_memops.hpp"
#include "_types.hpp"

namespace wombat {
namespace fs {

template<typename FsSize_t>
class FileStore {

	public:
		struct FsHeader {
			uint32_t version;
			FsSize_t size;
			FsSize_t rootInode;
		};

		typedef uint16_t InodeId_t;

		struct StatInfo {
			InodeId_t inodeId;
			FsSize_t  size;
		};

	private:
		struct Inode {
			// the next Inode in memory
			FsSize_t prev, next;
			FsSize_t left, right;
			FsSize_t dataLen;

			// The following variables should not be assumed to exist
			FsSize_t m_id;
			// must be last item
			uint8_t  m_data;

			FsSize_t size();
			void setId(InodeId_t);
			void setData(void *data, int size);

			private:
				Inode() = default;
		};

		uint8_t *m_begin, *m_end;
		uint32_t &m_version;
		Inode *m_root;

	public:
		/**
		 * Constructor
		 * @param begin pointer to the beginning of this FileStore's memory chunk
		 * @param end pointer to the end of this FileStore's memory chunk
		 * @param error pointer to a integer return errors into
		 */
		FileStore(uint8_t *begin, uint8_t *end, Error *error = nullptr);

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

		static uint8_t *format(uint8_t *buffer, FsSize_t size);

	private:
		/**
		 * Gets the header section of the file system.
		 * @return the header section of the file system.
		 */
		FsHeader *getHeader();

		/**
		 * Gets the record at the given id.
		 * @param root the root node to start comparing on
		 * @param id id of the "file"
		 * @param pathLen number of characters in pathLen
		 * @return the requested Inode, if available
		 */
		Inode *getRecord(FileStore::Inode *root, InodeId_t id);

		/**
		 * Gets an address for a new Inode.
		 * @param size the size of the Inode
		 */
		void *alloc(FsSize_t size);

		/**
		 * Compresses all of the records into a contiguous space, starting at m_root.
		 */
		void compress();

		/**
		 * Inserts the given insertValue into the tree of the given root.
		 * If the record already exists, it replaces the old on deletes it.
		 * @return true if the record was inserted
		 */
		bool insert(Inode *root, Inode *insertValue);

		/**
		 * Gets the FsSize_t associated with the next Inode to be allocated.
		 * @retrun the FsSize_t associated with the next Inode to be allocated
		 */
		FsSize_t iterator();

		/**
		 * Converts an actual pointer to a FsSize_t.
		 */
		FsSize_t ptr(void *ptr);

		/**
		 * Converts a FsSize_t to an actual pointer.
		 */
		template<typename T>
		T ptr(FsSize_t ptr) {
			return (T) (m_begin + ptr);
		};

		Inode *firstInode();

		Inode *lastInode();
};

template<typename FsSize_t>
FsSize_t FileStore<FsSize_t>::Inode::size() {
	return offsetof(FileStore::Inode, m_id) + dataLen;
}

template<typename FsSize_t>
void FileStore<FsSize_t>::Inode::setId(InodeId_t id) {
	this->m_id = id;
}

template<typename FsSize_t>
void FileStore<FsSize_t>::Inode::setData(void *data, int size) {
	memcpy(&m_data, data, size);
	dataLen = size;
}


// FileStore

template<typename FsSize_t>
FileStore<FsSize_t>::FileStore(uint8_t *begin, uint8_t *end, Error *error): m_version(*((uint32_t*) begin)) {
	if (version() != m_version) {
		// version mismatch
		if (error) {
			*error = 1;
		}
	} else {
		// ok
		m_begin = begin;
		m_end = end;
		auto header = (FsHeader*) m_begin;
		m_root = ptr<Inode*>(header->rootInode);
		if (error) {
			*error = header->size != (unsigned ) (m_end - m_begin);
		}
	}
}

template<typename FsSize_t>
void FileStore<FsSize_t>::init() {
	memset(m_begin, 0, m_end - m_begin);
	m_version = version();
}

template<typename FsSize_t>
int FileStore<FsSize_t>::write(void *data, FsSize_t dataLen) {
	return 1;
}

template<typename FsSize_t>
int FileStore<FsSize_t>::write(InodeId_t id, void *data, FsSize_t dataLen) {
	auto retval = 1;
	const FsSize_t size = offsetof(Inode, m_id) + dataLen;
	auto inode = (Inode*) alloc(size);
	if (inode) {
		inode->m_id = id;
		inode->setData(data, dataLen);
		if (insert(m_root, inode) || m_root == inode) {
			retval = 0;
		}
	}
	return retval;
}

template<typename FsSize_t>
int FileStore<FsSize_t>::read(InodeId_t id, void *data, FsSize_t *size) {
	auto rec = getRecord(m_root, id);
	int retval = 1;
	if (rec) {
		if (size) {
			*size = rec->dataLen;
		}
		memcpy(data, &rec->m_data, rec->dataLen);
		retval = 0;
	}
	return retval;
}

template<typename FsSize_t>
typename FileStore<FsSize_t>::StatInfo FileStore<FsSize_t>::stat(InodeId_t id) {
	auto inode = getRecord(m_root, id);
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
typename FileStore<FsSize_t>::FsHeader *FileStore<FsSize_t>::getHeader() {
	return (FsHeader*) m_begin;
}

template<typename FsSize_t>
typename FileStore<FsSize_t>::Inode *FileStore<FsSize_t>::getRecord(FileStore::Inode *root, InodeId_t id) {
	Inode *retval = nullptr;

	if (root->m_id > id) {
		if (root->left) {
			retval = getRecord(ptr<Inode*>(root->left), id);
		}
	} else if (root->m_id < id) {
		if (root->right) {
			retval = getRecord(ptr<Inode*>(root->right), id);
		}
	} else if (root->m_id == id) {
		retval = root;
	}

	return retval;
}

template<typename FsSize_t>
void *FileStore<FsSize_t>::alloc(FsSize_t size) {
	if ((lastInode()->next + size) > (uint64_t) m_end) {
		compress();
		if ((lastInode()->next + size) > (uint64_t) m_end) {
			return nullptr;
		}
	}

	const auto retval = lastInode()->next;
	const auto rec = ptr<Inode*>(retval);
	memset(rec, 0, size);
	rec->prev = ptr(lastInode());
	rec->next = retval + size;
	firstInode()->prev = retval;
	return rec;
}

template<typename FsSize_t>
void FileStore<FsSize_t>::compress() {
	auto current = m_root;
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
	return ((uint8_t*) ptr) - m_begin;
}

template<typename FsSize_t>
typename FileStore<FsSize_t>::Inode *FileStore<FsSize_t>::firstInode() {
	return ptr<Inode*>(sizeof(FsHeader));
}

template<typename FsSize_t>
typename FileStore<FsSize_t>::Inode *FileStore<FsSize_t>::lastInode() {
	return ptr<Inode*>(firstInode()->prev);
}

template<typename FsSize_t>
uint8_t FileStore<FsSize_t>::version() {
	return 0;
};

template<typename FsSize_t>
uint8_t *FileStore<FsSize_t>::format(uint8_t *buffer, FsSize_t size) {
	memset(buffer, 0, size);

	auto header = (FsHeader*) buffer;
	header->version = FileStore<FsSize_t>::version();
	header->size = size;
	header->rootInode = sizeof(FsHeader);

	auto inodeSection = (Inode*) (buffer + header->rootInode);
	inodeSection->m_id = 0;
	inodeSection->next = inodeSection->prev = (uint8_t*) inodeSection - (uint8_t*) buffer;

	return (uint8_t*) header;
}

typedef FileStore<uint16_t> FileStore16;
typedef FileStore<uint32_t> FileStore32;
typedef FileStore<uint64_t> FileStore64;

}
}

#endif
