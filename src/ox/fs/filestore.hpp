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

template<typename FsT>
class FileStore {

	public:
		typedef uint16_t InodeId_t;
		typedef FsT FsSize_t;

		struct StatInfo {
			InodeId_t inodeId;
			FsSize_t  size;
			uint8_t fileType;
		};

	private:
		struct Inode {
			// the next Inode in memory
			FsSize_t prev, next;
			FsSize_t dataLen;
			InodeId_t id;
			uint8_t refs;
			uint8_t fileType;
			FsSize_t left, right;

			FsSize_t size();
			void setId(InodeId_t);
			void setData(void *data, FsSize_t size);
			void *data();
		};

		uint32_t m_fsType;
		FsSize_t m_size;
		FsSize_t m_rootInode;

	public:
		/**
		 * Compacts and resizes the file store to the minimum possible size for
		 * the contents.
		 */
		void resize();

		/**
		 * Writes the given data to a "file" with the given id.
		 * @param id the id of the file
		 * @param data the contents of the file
		 * @param dataLen the number of bytes data points to
		 */
		int write(void *data, FsSize_t dataLen, uint8_t fileType = 0);

		/**
		 * Writes the given data to a "file" with the given id.
		 * @param id the id of the file
		 * @param data the contents of the file
		 * @param dataLen the number of bytes data points to
		 */
		int write(InodeId_t id, void *data, FsSize_t dataLen, uint8_t fileType = 0);

		/**
		 * Removes the inode of the given ID.
		 * @param id the id of the file
		 */
		int remove(InodeId_t id);

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

		/**
		 * Returns the size of the file store.
		 * @return the size of the file store.
		 */
		FsSize_t size();

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
		 * Gets the inode at the given id.
		 * @param root the root node to start comparing on
		 * @param id id of the "file"
		 * @param pathLen number of characters in pathLen
		 * @param targetAddr the address of the target inode
		 * @return the requested Inode, if available
		 */
		Inode *getInodeParent(Inode *root, InodeId_t id, FsSize_t targetAddr);

		/**
		 * Removes the inode of the given ID.
		 * @param id the id of the file
		 */
		int remove(Inode *root, InodeId_t id);

		/**
		 * Removes the given node from the linked list.
		 * @param node node to remove
		 */
		void unlink(Inode *node);

		/**
		 * Gets the address of the next available inode, assuming there is a next
		 * available inode.
		 */
		FsSize_t nextInodeAddr();

		/**
		 * Gets an address for a new Inode.
		 * @param size the size of the Inode
		 */
		void *alloc(FsSize_t size);

		/**
		 * Compacts all of the inodes into a contiguous space, starting at the first inode.
		 */
		void compact();

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

		FsSize_t firstInode();

		Inode *lastInode();

		/**
		 * Updates the address of the inode in the tree.
		 */
		void updateInodeAddress(InodeId_t id, FsSize_t oldAddr, FsSize_t newAddr);

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
	this->id = id;
}

template<typename FsSize_t>
void FileStore<FsSize_t>::Inode::setData(void *data, FsSize_t size) {
	ox_memcpy(this->data(), data, size);
	dataLen = size;
}


template<typename FsSize_t>
void *FileStore<FsSize_t>::Inode::data() {
	return this + 1;
}


// FileStore

template<typename FsSize_t>
void FileStore<FsSize_t>::resize() {
	compact();
	auto last = lastInode();
	m_size = ptr(last) + last->size();
}

template<typename FsSize_t>
int FileStore<FsSize_t>::write(void *data, FsSize_t dataLen, uint8_t fileType) {
	return 1;
}

template<typename FsSize_t>
int FileStore<FsSize_t>::write(InodeId_t id, void *data, FsSize_t dataLen, uint8_t fileType) {
	auto retval = 1;
	const FsSize_t size = sizeof(Inode) + dataLen;
	auto inode = (Inode*) alloc(size);
	if (inode) {
		remove(id);
		inode->id = id;
		inode->fileType = fileType;
		inode->setData(data, dataLen);
		auto root = ptr<Inode*>(m_rootInode);
		if (insert(root, inode) || root == inode) {
			retval = 0;
		}
	}
	return retval;
}

template<typename FsSize_t>
int FileStore<FsSize_t>::remove(InodeId_t id) {
	return remove(ptr<Inode*>(m_rootInode), id);
}

template<typename FsSize_t>
int FileStore<FsSize_t>::remove(Inode *root, InodeId_t id) {
	auto err = 1;

	if (root->id > id) {
		if (root->left) {
			auto node = ptr<Inode*>(root->left);
			if (node->id != id) {
				err = remove(node, id);
			} else {
				root->left = 0;
				if (node->right) {
					insert(root, ptr<Inode*>(node->right));
				}
				if (node->left) {
					insert(root, ptr<Inode*>(node->left));
				}
				unlink(node);
				node->id = 0;
				node->left = 0;
				node->right = 0;
				err = 0;
			}
		}
	} else if (root->id < id) {
		if (root->right) {
			auto node = ptr<Inode*>(root->right);
			if (node->id != id) {
				err = remove(node, id);
			} else {
				root->right = 0;
				if (node->right) {
					insert(root, ptr<Inode*>(node->right));
				}
				if (node->left) {
					insert(root, ptr<Inode*>(node->left));
				}
				unlink(node);
				node->id = 0;
				node->left = 0;
				node->right = 0;
				err = 0;
			}
		}
	} else if (ptr<Inode*>(m_rootInode)->id == id) {
		m_rootInode = root->right;
		if (root->left) {
			insert(ptr<Inode*>(m_rootInode), ptr<Inode*>(root->left));
		}
		unlink(root);
		root->id = 0;
		root->left = 0;
		root->right = 0;
		err = 0;
	}

	return err;
}

template<typename FsSize_t>
void FileStore<FsSize_t>::unlink(Inode *inode) {
	auto next = ptr<Inode*>(inode->next);
	auto prev = ptr<Inode*>(inode->prev);
	prev->next = ptr(next);
	next->prev = ptr(prev);

	ox_memset(inode, 0, inode->size());
	inode->prev = firstInode();
	inode->next = firstInode();
}

template<typename FsSize_t>
void FileStore<FsSize_t>::updateInodeAddress(InodeId_t id, FsSize_t oldAddr, FsSize_t newAddr) {
	auto parent = getInodeParent(ptr<Inode*>(m_rootInode), id, oldAddr);
	if (parent) {
		if (parent->left == oldAddr) {
			parent->left = newAddr;
		} else if (parent->right == oldAddr) {
			parent->right = newAddr;
		}
	}
}

template<typename FsSize_t>
int FileStore<FsSize_t>::read(InodeId_t id, void *data, FsSize_t *size) {
	auto inode = getInode(ptr<Inode*>(m_rootInode), id);
	int retval = 1;
	if (inode) {
		if (size) {
			*size = inode->dataLen;
		}
		ox_memcpy(data, inode->data(), inode->dataLen);
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
		stat.fileType = inode->fileType;
		stat.inodeId = id;
	} else {
		stat.inodeId = 0;
	}
	return stat;
}

template<typename FsSize_t>
FsSize_t FileStore<FsSize_t>::size() {
	return m_size;
}

template<typename FsSize_t>
typename FileStore<FsSize_t>::Inode *FileStore<FsSize_t>::getInode(Inode *root, InodeId_t id) {
	Inode *retval = nullptr;

	if (root->id > id) {
		if (root->left) {
			retval = getInode(ptr<Inode*>(root->left), id);
		}
	} else if (root->id < id) {
		if (root->right) {
			retval = getInode(ptr<Inode*>(root->right), id);
		}
	} else if (root->id == id) {
		retval = root;
	}

	return retval;
}

template<typename FsSize_t>
typename FileStore<FsSize_t>::Inode *FileStore<FsSize_t>::getInodeParent(Inode *root, InodeId_t id, FsSize_t targetAddr) {
	Inode *retval = nullptr;

	if (root->id > id) {
		if (root->left) {
			if (root->left == targetAddr) {
				retval = root;
			} else {
				retval = getInodeParent(ptr<Inode*>(root->left), id, targetAddr);
			}
		}
	} else if (root->id < id) {
		if (root->right) {
			if (root->right == targetAddr) {
				retval = root;
			} else {
				retval = getInodeParent(ptr<Inode*>(root->right), id, targetAddr);
			}
		}
	}

	return retval;
}

template<typename FsSize_t>
FsSize_t FileStore<FsSize_t>::nextInodeAddr() {
	FsSize_t next = ptr(lastInode()) + lastInode()->size();
	return next;
}

template<typename FsSize_t>
void *FileStore<FsSize_t>::alloc(FsSize_t size) {
	FsSize_t next = nextInodeAddr();
	if ((next + size) > (uint64_t) end()) {
		compact();
		next = nextInodeAddr();
		if ((next + size) > (uint64_t) end()) {
			return nullptr;
		}
	}

	const auto retval = next;
	const auto inode = ptr<Inode*>(retval);
	ox_memset(inode, 0, size);
	inode->prev = ptr<Inode*>(firstInode())->prev;
	inode->next = retval + size;
	ptr<Inode*>(firstInode())->prev = retval;
	return inode;
}

template<typename FsSize_t>
void FileStore<FsSize_t>::compact() {
	auto dest = ptr<Inode*>(firstInode());
	auto current = ptr<Inode*>(firstInode());
	while (current->next > firstInode() && current->next < ptr(end())) {
		ox_memcpy(dest, current, current->size());
		if (dest->next != firstInode()) {
			dest->next = ptr(dest) + dest->size();
		}
		ptr<Inode*>(dest->next)->prev = ptr(dest);
		updateInodeAddress(dest->id, ptr(current), ptr(dest));
		current = ptr<Inode*>(dest->next);
		dest = ptr<Inode*>(ptr(dest) + dest->size());
	}
}

template<typename FsSize_t>
bool FileStore<FsSize_t>::insert(Inode *root, Inode *insertValue) {
	auto retval = false;

	if (root->id > insertValue->id) {
		if (root->left) {
			retval = insert(ptr<Inode*>(root->left), insertValue);
		} else {
			root->left = ptr(insertValue);
			retval = true;
		}
	} else if (root->id < insertValue->id) {
		if (root->right) {
			retval = insert(ptr<Inode*>(root->right), insertValue);
		} else {
			root->right = ptr(insertValue);
			retval = true;
		}
	} else if (m_rootInode == 0) {
		m_rootInode = ptr(insertValue);
		retval = true;
	}

	return retval;
}

template<typename FsSize_t>
FsSize_t FileStore<FsSize_t>::iterator() {
	return ptr(lastInode()) + lastInode()->size();
}

template<typename FsSize_t>
FsSize_t FileStore<FsSize_t>::ptr(void *ptr) {
#ifdef _MSC_VER
#pragma warning(disable:4244)
#endif
	return ((uint8_t*) ptr) - begin();
#ifdef _MSC_VER
#pragma warning(default:4244)
#endif
}

template<typename FsSize_t>
FsSize_t FileStore<FsSize_t>::firstInode() {
	return sizeof(FileStore<FsSize_t>);
}

template<typename FsSize_t>
typename FileStore<FsSize_t>::Inode *FileStore<FsSize_t>::lastInode() {
	return ptr<Inode*>(ptr<Inode*>(firstInode())->prev);
}

template<typename FsSize_t>
uint8_t FileStore<FsSize_t>::version() {
	return 1;
};

template<typename FsSize_t>
uint8_t *FileStore<FsSize_t>::format(uint8_t *buffer, FsSize_t size, uint32_t fsType) {
	ox_memset(buffer, 0, size);

	auto *fs = (FileStore*) buffer;
	fs->m_fsType = fsType;
	fs->m_size = size;
	fs->m_rootInode = sizeof(FileStore<FsSize_t>);
	((Inode*) (fs + 1))->prev = fs->firstInode();
	fs->lastInode()->next = sizeof(FileStore<FsSize_t>);

	return (uint8_t*) buffer;
}

typedef FileStore<uint16_t> FileStore16;
typedef FileStore<uint32_t> FileStore32;
typedef FileStore<uint64_t> FileStore64;

}
}
