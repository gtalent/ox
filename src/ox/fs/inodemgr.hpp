/*
 * Copyright 2015 - 2017 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#pragma once

namespace ox {
namespace fs {

/**
 * Used to track unused inodes.
 */
template<typename InodeId_t>
struct InodeFile {
	InodeId_t length;

	InodeId_t *inodes();
};

template<typename InodeId_t>
InodeId_t *InodeFile<InodeId_t>::inodes() {
	return (InodeId_t*) (this + 1);
}

template<typename InodeId_t>
int generateInodeFile(InodeId_t *inodes, int count) {
	const static InodeId_t endPoint = ~0;
	auto it = count;
	auto multiplier = 1;
	auto current = endPoint / 2;
	auto lowestNode = current;

	inodes[0] = count;

	while (it > 0) {
		current = lowestNode * multiplier;
		if (current < endPoint) {
			inodes[it--] = current;
			multiplier += 2;
		} else {
			multiplier = 1;
			lowestNode /= 2;
		}
	}

	return 0;
}

}
}
