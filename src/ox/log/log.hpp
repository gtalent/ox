/*
 * Copyright 2015 - 2017 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

namespace ox {

enum class LogLevel_t: int {
	Info,
	Debug,
	Error,
};

void logFile(const char *path);

void info(const char *msg, ...);

void debug(const char *msg, ...);

void error(const char *msg, ...);

}
