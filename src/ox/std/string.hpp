/*
 * Copyright 2015 - 2017 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include "types.hpp"

namespace ox {

// Bounded String
template<size_t buffLen>
class bstring {
	private:
		uint8_t m_buff[buffLen];

	public:
		bstring(const char *str);

		const bstring &operator=(const char *str);

		const bstring &operator=(char *str);

		/**
		 * Returns the number of characters in this string.
		 */
		size_t len();

		/**
		 * Returns the number of bytes used for this string.
		 */
		size_t size();
};

template<size_t size>
bstring<size>::bstring(const char *str) {
	*this = str;
}

template<size_t size>
const bstring<size> &bstring<size>::operator=(const char *str) {
	return *this = (const char*) str;
}

template<size_t size>
const bstring<size> &bstring<size>::operator=(char *str) {
	auto strLen = ox_strlen(str) + 1;
	if (size() < strLen) {
		strLen = size();
	}
	ox_memcpy(m_buff, str, strLen);
	// make sure last element is a null terminator
	m_buff[size() - 1] = 0;
	return *this;
}

template<size_t buffLen>
size_t bstring<buffLen>::len() {
	size_t length = 0;
	for (size_t i = 0; i < buffLen; i++) {
		auto b = m_buff[i];
		if (b) {
			if ((b & (1 << 8)) == 0) { // normal ASCII character
				length++;
			} else if ((b & (256 << 6)) == (256 << 6)) { // start of UTF-8 character
				length++;
			}
		} else {
			break;
		}
	}
	return length;
}

template<size_t buffLen>
size_t bstring<buffLen>::size() {
	return buffLen;
}

}
