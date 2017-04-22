/*
 * Copyright 2015 - 2017 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <ox/std/memops.hpp>
#include <ox/std/strops.hpp>
#include "pathiterator.hpp"

namespace ox {
namespace fs {

PathIterator::PathIterator(const char *path, size_t maxSize) {
	m_path = path;
	m_maxSize = maxSize;
}

int PathIterator::next(char *pathOut, size_t pathOutSize) {
	int size = 0;
	const char *substr = ox_strchar(m_path + m_iterator, '/', m_maxSize - m_iterator);
	m_iterator = (substr - m_path) + 1;
	if (substr && m_iterator < m_maxSize) {
		int start = m_iterator;
		int end = (ox_strchar(m_path + start, '/', m_maxSize - start) - m_path);
		if (end < 0) {
			end = m_maxSize;
		}
		size = end - start;
		ox_memcpy(pathOut, &m_path[start], size);
	}
	pathOut[size] = 0; // end with null terminator
	return 0;
}

}
}
