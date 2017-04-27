/*
 * Copyright 2015 - 2017 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <ox/std/byteswap.hpp>
#include <ox/std/memops.hpp>
#include "write.hpp"

namespace ox {
namespace mc {

int FieldPresenseMask::set(int i, bool on) {
	uint8_t val = on ? 1 : 0; // normalize to 0 or 1
	if (i / 8 < m_maxLen) {
		m_mask[i / 8] |= ox::std::bigEndianAdapt((uint8_t) (val << (i % 8)));
		return 0;
	} else {
		return MC_PRESENCEMASKOUTBOUNDS;
	}
}

void FieldPresenseMask::setMaxLen(int maxLen) {
	m_maxLen = maxLen;
}



MetalClawWriter::MetalClawWriter(uint8_t *buff, size_t buffLen): m_fieldPresence(buff, buffLen) {
	m_buff = buff;
	m_buffLen = buffLen;
}

int MetalClawWriter::op(const char*, int16_t *val) {
	return appendInteger(*val);
}

int MetalClawWriter::op(const char*, int32_t *val) {
	return appendInteger(*val);
}

int MetalClawWriter::op(const char*, int64_t *val) {
	return appendInteger(*val);
}

int MetalClawWriter::op(const char*, uint16_t *val) {
	return appendInteger(*val);
}

int MetalClawWriter::op(const char*, uint32_t *val) {
	return appendInteger(*val);
}

int MetalClawWriter::op(const char*, uint64_t *val) {
	return appendInteger(*val);
}

int MetalClawWriter::op(const char*, bool *val) {
	return m_fieldPresence.set(m_field++, *val);
}

int MetalClawWriter::op(const char*, const char **val, size_t len) {
	int err = 0;
	if (val) {
		// write the length
		typedef uint32_t StringLength;
		if (m_buffIt + sizeof(StringLength) + len < m_buffLen) {
			*((StringLength*) &m_buff[m_buffIt]) = ox::std::bigEndianAdapt((StringLength) len);
			m_field++;
			m_buffIt += sizeof(StringLength);

			// write the string
			ox_memcpy(&m_buff[m_buffIt], val, len);
			m_buffIt += len + 1;
			m_buff[m_buffIt - 1] = 0;
			err |= m_fieldPresence.set(m_field, true);
			m_field++;
		}
	}
	return err;
}

void MetalClawWriter::setFields(int fields) {
	m_fields = fields;
	m_field = fields / 8;
	m_fieldPresence.setMaxLen(fields / 8);
}

}
}
