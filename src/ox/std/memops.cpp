/*
 * Copyright 2015 - 2016 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include "memops.hpp"

void *ox_memcpy(void *dest, const void *src, size_t size) {
	char *srcBuf = (char*) src;
	char *dstBuf = (char*) dest;
	for (size_t i = 0; i < size; i++) {
		dstBuf[i] = (char) srcBuf[i];
	}
	return dest;
}

void *ox_memset(void *ptr, int val, size_t size) {
	char *buf = (char*) ptr;
	for (size_t i = 0; i < size; i++) {
		buf[i] = val;
	}
	return ptr;
}
