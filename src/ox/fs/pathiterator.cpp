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
	size_t size = 0;
	int retval = 1;
	if (m_iterator < m_maxSize && ox_strlen(&m_path[m_iterator])) {
		retval = 0;
		if (m_path[m_iterator] == '/') {
			m_iterator++;
		}
		size_t start = m_iterator;
		// end is at the next /
		const char *substr = ox_strchr(&m_path[start], '/', m_maxSize - start);
		// correct end if it is invalid, which happens if there is no next /
		if (!substr) {
			substr = ox_strchr(&m_path[start], 0, m_maxSize - start);
		}
		size_t end = substr - m_path;
		size = end - start;
		ox_memcpy(pathOut, &m_path[start], size);
	}
	pathOut[size] = 0; // end with null terminator
	m_iterator += size;
	return retval;
}

bool PathIterator::hasNext() {
	size_t size = 0;
	if (m_iterator < m_maxSize && ox_strlen(&m_path[m_iterator])) {
		size_t start = m_iterator;
		// end is at the next /
		const char *substr = ox_strchr(&m_path[start], '/', m_maxSize - start);
		// correct end if it is invalid, which happens if there is no next /
		if (!substr) {
			substr = ox_strchr(&m_path[start], 0, m_maxSize - start);
		}
		size_t end = substr - m_path;
		size = end - start;
	}
	m_iterator += size;
	return size > 0;
}

}
}
