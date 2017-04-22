/*
 * Copyright 2015 - 2017 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "strops.hpp"

int ox_strcmp(const char *str1, const char *str2) {
	auto retval = 0;
	auto i = 0;
	while (str1[i] || str2[i]) {
		if (str1[i] < str2[i]) {
			retval = -1;
			break;
		} else if (str1[i] > str2[i]) {
			retval = 1;
			break;
		}
		i++;
	}
	return retval;
}

int ox_strlen(const char *str1) {
	int len;
	for (len = 0; str1[len]; len++);
	return len;
}

const char *ox_strchar(const char *str, int character, size_t maxLen) {
	for (size_t i = 0; i < maxLen && str[i]; i++) {
		if (str[i] == character) {
			return &str[i];
		}
	}
	return nullptr;
}

char *ox_strchar(char *str, int character, size_t maxLen) {
	for (size_t i = 0; i < maxLen && str[i]; i++) {
		if (str[i] == character) {
			return &str[i];
		}
	}
	return nullptr;
}

int ox_atoi(const char *str) {
	int total = 0;
	int multiplier = 1;

	for (auto i = ox_strlen(str) - 1; i != -1; i--) {
		total += (str[i] - '0') * multiplier;
		multiplier *= 10;
	}

	return total;
}
