/*
 * Copyright 2015 - 2017 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <ox/std/types.hpp>

namespace ox {
namespace mc {

enum {
	MC_PRESENCEMASKOUTBOUNDS = 1,
	MC_BUFFENDED = 2
};

class FieldPresenseMask {
	private:
		uint8_t *m_mask;
		int m_maxLen = 0;

	public:
		FieldPresenseMask(uint8_t *mask, size_t maxLen);

		int set(int i, bool on);

		void setMaxLen(int);
};

class MetalClawWriter {

	private:
		FieldPresenseMask m_fieldPresence;
		int m_fields = 0;
		int m_field = 0;
		size_t m_buffIt = 0;
		size_t m_buffLen = 0;
		uint8_t *m_buff = nullptr;

	public:
		MetalClawWriter(uint8_t *buff, size_t buffLen);

		int op(const char*, int16_t *val);
		int op(const char*, int32_t *val);
		int op(const char*, int64_t *val);

		int op(const char*, uint16_t *val);
		int op(const char*, uint32_t *val);
		int op(const char*, uint64_t *val);

		int op(const char*, bool *val);

		/**
		 *
		 * @param len the length of the string, not including the null terminator
		 */
		int op(const char*, const char **val, size_t len);

		template<typename T>
		int op(const char*, T **val, size_t len);

		template<typename T>
		int op(const char*, T *val);

		void setFields(int fields);

	private:
		template<typename I>
		int appendInteger(I val);
};

template<typename T>
int MetalClawWriter::op(const char*, T *val) {
	MetalClawWriter reader(m_buff, m_buffLen - m_buffLen);
	return ioOp(&reader, val);
};

template<typename I>
int MetalClawWriter::appendInteger(I val) {
	int err = 0;
	if (val) {
		if (m_buffIt + sizeof(I) < m_buffLen) {
			*((I*) &m_buff[m_buffIt]) = ox::std::bigEndianAdapt(val);
			err |= m_fieldPresence.set(m_field, true);
			m_field++;
			m_buffIt += sizeof(I);
		} else {
			err = MC_BUFFENDED;
		}
	}
	return err;
};

template<typename T>
int MetalClawWriter::op(const char *fieldName, T **val, size_t len) {
	int err = 0;
	if (val) {
		// write the length
		typedef uint32_t ArrayLength;
		if (m_buffIt + sizeof(ArrayLength) < m_buffLen) {
			*((ArrayLength*) &m_buff[m_buffIt]) = ox::std::bigEndianAdapt((ArrayLength) len);
			m_field++;
			m_buffIt += sizeof(ArrayLength);
		} else {
			err = MC_BUFFENDED;
		}

		// write the string
		for (size_t i = 0; i < len; i++) {
			err |= op(val[i]);
		}
	}
	return err;
};

template<typename T>
int read(uint8_t *buff, size_t buffLen, T *val) {
	MetalClawWriter writer(buff, buffLen);
	return ioOp(&writer, val);
}

}
}
