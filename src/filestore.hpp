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
			FsSize_t rootInode = 0;
			bool insertLeft = false; // crude tree balancing mechanism
		};

		typedef uint16_t InodeId_t;

	private:
		struct Inode {
			// the next Inode in memory
			FsSize_t prev, next;
			FsSize_t left, right;
			FsSize_t dataLen;
			// offsets from Inode this
			FsSize_t m_id;
			FsSize_t m_data;

			FsSize_t size();
			void setId(InodeId_t);
			void setData(uint8_t *data, int size);

			private:
				Inode() = default;
		};

		uint8_t *m_begin, *m_end;
		uint32_t &m_version;
		// the last Inode in the FileStore's memory chunk
		FsSize_t &m_lastRec;
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
		bool insert(Inode *root, Inode *insertValue, FsSize_t *rootParentPtr = 0);

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
			return (T) m_begin + ptr;
		};
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
void FileStore<FsSize_t>::Inode::setData(uint8_t *data, int size) {
	memcpy(this + m_data, data, size);
	m_data = size;
}


// FileStore

template<typename FsSize_t>
FileStore<FsSize_t>::FileStore(uint8_t *begin, uint8_t *end, Error *error): m_version(*((uint32_t*) begin)), m_lastRec(*(FsSize_t*) (begin + sizeof(FsHeader))) {
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
			*error = header->size != m_end - m_begin;
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
	auto rec = (Inode*) alloc(size);
	if (rec) {
		rec->dataLen = dataLen;
		if (insert(m_root, rec)) {
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
		memcpy(data, ptr<uint8_t*>(rec->m_data), rec->dataLen);
		retval = 0;
	}
	return retval;
}

template<typename FsSize_t>
typename FileStore<FsSize_t>::FsHeader *FileStore<FsSize_t>::getHeader() {
	return (FsHeader*) m_begin;
}

template<typename FsSize_t>
typename FileStore<FsSize_t>::Inode *FileStore<FsSize_t>::getRecord(FileStore::Inode *root, InodeId_t id) {
	auto cmp = root->m_id > id;
	FsSize_t recPt;
	if (cmp) {
		recPt = root->left;
	} else if (!cmp) {
		recPt = root->right;
	} else {
		recPt = ptr(root);
	}
	if (recPt) {
		return getRecord(ptr<Inode*>(recPt), id);
	} else {
		return ptr<Inode*>(recPt);
	}
}

template<typename FsSize_t>
void *FileStore<FsSize_t>::alloc(FsSize_t size) {
	const auto iterator = this->iterator();
	if ((iterator + size) > (uint64_t) m_end) {
		compress();
		if ((iterator + size) > (uint64_t) m_end) {
			return nullptr;
		}
	}
	ptr<Inode*>(m_lastRec)->next = iterator;

	auto rec = ptr<uint8_t*>(iterator);
	memset(rec, 0, size);
	ptr<Inode*>(iterator)->prev = m_lastRec;
	m_lastRec = iterator;
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
bool FileStore<FsSize_t>::insert(Inode *root, Inode *insertValue, FsSize_t *rootParentPtr) {
	auto cmp = root->m_id > insertValue->m_id;
	if (cmp) {
		if (root->left) {
			return insert(ptr<Inode*>(root->left), insertValue, &root->left);
		} else {
			root->left = ((uint8_t*) insertValue) - m_begin;
			return true;
		}
	} else if (!cmp) {
		if (root->right) {
			return insert(ptr<Inode*>(root->right), insertValue, &root->right);
		} else {
			root->right = ((uint8_t*) insertValue) - m_begin;
			return true;
		}
	} else {
		auto ivAddr = ((uint8_t*) insertValue) - m_begin;
		if (root->prev) {
			ptr<Inode*>(root->prev)->next = ivAddr;
		}
		if (root->next) {
			ptr<Inode*>(root->next)->prev = ivAddr;
		}
		if (rootParentPtr) {
			*rootParentPtr = ivAddr;
		}
		return true;
	}
	return false;
}

template<typename FsSize_t>
FsSize_t FileStore<FsSize_t>::iterator() {
	return m_lastRec + ((Inode*) m_begin + m_lastRec)->size();
}

template<typename FsSize_t>
FsSize_t FileStore<FsSize_t>::ptr(void *ptr) {
	return ((uint8_t*) ptr) - m_begin;
}

template<typename FsSize_t>
uint8_t FileStore<FsSize_t>::version() {
	return 0;
};

template<typename FsSize_t>
uint8_t *FileStore<FsSize_t>::format(uint8_t *buffer, FsSize_t size) {
	auto header = (FsHeader*) buffer;
	header->version = FileStore<FsSize_t>::version();
	header->size = size;
	return (uint8_t*) header;
}

typedef FileStore<uint16_t> FileStore16;
typedef FileStore<uint32_t> FileStore32;
typedef FileStore<uint64_t> FileStore64;

}
}

#endif
