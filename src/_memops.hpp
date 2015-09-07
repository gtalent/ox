/*
 * Copyright 2015 gtalent2@gmail.com
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef MEMPHIS_MEMOPS_HPP
#define MEMPHIS_MEMOPS_HPP

namespace memphis {

void memcpy(void *src, void *dest, int size);

void memset(void *ptr, char val, int size);

}

#endif
