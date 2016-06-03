/*
 * Copyright 2015 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef WOMBAT_FS_MEMOPS_HPP
#define WOMBAT_FS_MEMOPS_HPP

namespace wombat {
namespace fs {

void memcpy(void *src, void *dest, int size);

void memset(void *ptr, char val, int size);

}
}

#endif
