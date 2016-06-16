/*
 * Copyright 2015 - 2016 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
namespace wombat {
namespace fs {

int strcmp(const char *str1, const char *str2) {
	auto retval = 0;
	auto i = 0;
	do {
		if (str1[i] < str2[i]) {
			retval = -1;
			break;
		} else if (str1[i] > str2[i]) {
			retval = 1;
			break;
		}
		i++;
	} while (str1[i] == str2[i] && str1[i]);
	return retval;
}

}
}

