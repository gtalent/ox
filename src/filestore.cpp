/*
 * Copyright 2015 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include <stdlib.h>
#include "_memops.hpp"
#include "filestore.hpp"

namespace wombat {
namespace fs {

uint32_t FileStore::version = 0;

uint8_t *initFs(uint8_t *buffer, size_t size, bool hasDirectories) {
	auto fs = (MemFsHeader*) (buffer ? buffer : malloc(size));
	fs->version = FileStore::version;
	return (uint8_t*) fs;
}

MemFsPtr FileStore::Record::size() {
	return offsetof(FileStore::Record, m_id) + dataLen;
}

void FileStore::Record::setId(RecordId id) {
	this->m_id = id;
}

void FileStore::Record::setData(uint8_t *data, int size) {
	memcpy(this + m_data, data, size);
	m_data = size;
}


// FileStore

FileStore::FileStore(uint8_t *begin, uint8_t *end, Error *error): m_version(*((uint32_t*) begin)), m_lastRec(*(MemFsPtr*) (begin + sizeof(m_version))) {
	if (version != m_version) {
		// version mismatch
		if (error) {
			*error = 1;
		}
	} else {
		// ok
		m_begin = begin;
		m_end = end;
		m_root = (Record*) (begin + sizeof(MemFsPtr));
		if (error) {
			*error = 0;
		}
	}
}

void FileStore::init() {
	memset(m_begin, 0, m_end - m_begin);
	m_version = version;
}

void FileStore::write(RecordId id, uint8_t *data, MemFsPtr dataLen) {
	const MemFsPtr size = offsetof(FileStore::Record, m_id) + dataLen;
	auto rec = (Record*) alloc(size);
	rec->dataLen = dataLen;
	insert(m_root, rec);
}

int FileStore::read(RecordId id, uint8_t **data, MemFsPtr *size) {
	auto rec = getRecord(m_root, id);
	int retval = 1;
	if (rec) {
		*size = rec->dataLen;
		*data = (uint8_t*) malloc(*size);
		memcpy(*data, ptr<uint8_t*>(rec->m_data), *size);
		retval = 0;
	}
	return retval;
}

FileStore::Record *FileStore::getRecord(FileStore::Record *root, RecordId id) {
	auto cmp = root->m_id > id;
	MemFsPtr recPt;
	if (cmp) {
		recPt = root->left;
	} else if (!cmp) {
		recPt = root->right;
	} else {
		recPt = ptr(root);
	}
	if (recPt) {
		return getRecord(ptr<Record*>(recPt), id);
	} else {
		return ptr<Record*>(recPt);
	}
}

void *FileStore::alloc(MemFsPtr size) {
	const auto iterator = this->iterator();
	if ((iterator + size) > (uint64_t) m_end) {
		compress();
		if ((iterator + size) > (uint64_t) m_end) {
			return nullptr;
		}
	}
	ptr<Record*>(m_lastRec)->next = iterator;

	auto rec = ptr<uint8_t*>(iterator);
	memset(rec, 0, size);
	ptr<Record*>(iterator)->prev = m_lastRec;
	m_lastRec = iterator;
	return rec;
}

void FileStore::compress() {
	auto current = m_root;
	while (current->next) {
		auto prevEnd = current + current->size();
		current = ptr<Record*>(current->next);
		if (prevEnd != current) {
			memcpy(prevEnd, current, current->size());
			current = prevEnd;
		}
	}
}

bool FileStore::insert(Record *root, Record *insertValue, MemFsPtr *rootParentPtr) {
	auto cmp = root->m_id > insertValue->m_id;
	if (cmp) {
		if (root->left) {
			return insert(ptr<Record*>(root->left), insertValue, &root->left);
		} else {
			root->left = ((uint8_t*) insertValue) - m_begin;
			return true;
		}
	} else if (!cmp) {
		if (root->right) {
			return insert(ptr<Record*>(root->right), insertValue, &root->right);
		} else {
			root->right = ((uint8_t*) insertValue) - m_begin;
			return true;
		}
	} else {
		auto ivAddr = ((uint8_t*) insertValue) - m_begin;
		if (root->prev) {
			ptr<Record*>(root->prev)->next = ivAddr;
		}
		if (root->next) {
			ptr<Record*>(root->next)->prev = ivAddr;
		}
		if (rootParentPtr) {
			*rootParentPtr = ivAddr;
		}
		return true;
	}
	return false;
}

MemFsPtr FileStore::iterator() {
	return m_lastRec + ((Record*) m_begin + m_lastRec)->size();
}

MemFsPtr FileStore::ptr(void *ptr) {
	return ((uint8_t*) ptr) - m_begin;
}

}
}
