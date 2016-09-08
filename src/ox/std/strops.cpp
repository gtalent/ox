/*
 * Copyright 2015 - 2016 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "strops.hpp"

int ox_strcmp(const char *str1, const char *str2) {
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

size_t ox_strlen(const char *str1) {
	int len;
	for (len = 0; str1[len]; len++);
	return len;
}

int ox_atoi(const char *str) {
	int total = 0;
	int multiplier = 1;

	for (auto i = ox_strlen(str) - 1; i != 0; i--) {
		total += (str[i] - '0') * multiplier;
		multiplier *= 10;
	}

	return total;
}
