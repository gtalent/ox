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
namespace std {


inline uint16_t byteSwap(uint16_t i) {
	return (i << 8) | (i >> 8);
}

inline uint32_t byteSwap(uint32_t i) {
	return ((i >> 24) & 0x000000ff) |
	       ((i >>  8) & 0x0000ff00) |
	       ((i <<  8) & 0x00ff0000) |
	       ((i << 24) & 0xff000000);
}

inline uint64_t byteSwap(uint64_t i) {
	return ((i >> 56) & 0x00000000000000ff) |
	       ((i >> 40) & 0x000000000000ff00) |
	       ((i >> 24) & 0x0000000000ff0000) |
	       ((i >>  8) & 0x00000000ff000000) |
	       ((i <<  8) & 0x000000ff00000000) |
	       ((i << 24) & 0x0000ff0000000000) |
	       ((i << 40) & 0x00ff000000000000) |
	       ((i << 56) & 0xff00000000000000);
}


/**
 * Takes a little endian int and byte swaps if the platform is big endian.
 */
inline uint8_t littleEndianToNative(uint8_t i) {
	return i;
}

/**
 * Takes a little endian int and byte swaps if the platform is big endian.
 */
inline uint16_t littleEndianToNative(uint16_t i) {
#ifdef __BIG_ENDIAN__
	return byteSwap(i);
#else
	return i;
#endif
}

/**
 * Takes a little endian int and byte swaps if the platform is big endian.
 */
inline uint32_t littleEndianToNative(uint32_t i) {
#ifdef __BIG_ENDIAN__
	return byteSwap(i);
#else
	return i;
#endif
}

/**
 * Takes a little endian int and byte swaps if the platform is big endian.
 */
inline uint64_t littleEndianToNative(uint64_t i) {
#ifdef __BIG_ENDIAN__
	return byteSwap(i);
#else
	return i;
#endif
}


inline uint8_t toLittleEndian(uint8_t i) {
	return i;
}

inline uint16_t toLittleEndian(uint16_t i) {
#ifdef __BIG_ENDIAN__
	return byteSwap(i);
#else
	return i;
#endif
}

inline uint32_t toLittleEndian(uint32_t i) {
#ifdef __BIG_ENDIAN__
	return byteSwap(i);
#else
	return i;
#endif
}

inline uint64_t toLittleEndian(uint64_t i) {
#ifdef __BIG_ENDIAN__
	return byteSwap(i);
#else
	return i;
#endif
}

}
}
