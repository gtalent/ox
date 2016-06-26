/*
 * Copyright 2015 - 2016 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#pragma once

namespace ox {
namespace std {

typedef char               int8_t;
typedef unsigned char      uint8_t;
typedef short              int16_t;
typedef unsigned short     uint16_t;
typedef int                int32_t;
typedef unsigned int       uint32_t;
typedef unsigned           uint_t;
typedef long long          int64_t;
typedef unsigned long long uint64_t;

typedef uint32_t Error;

#if defined(_LP64) || defined(__ppc64__) || defined(_WIN64) || defined(__aarch64__)
typedef uint64_t size_t;
#elif defined(_LP32) || defined(__ppc__) || defined(_WIN32) || defined(__arm__)
typedef uint32_t size_t;
#else
#error size_t undefined
#endif

}
}