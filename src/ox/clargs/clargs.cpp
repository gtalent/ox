/*
 * Copyright 2015 - 2017 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "clargs.hpp"

namespace ox {
namespace clargs {

ClArgs::ClArgs(int argc, const char **args) {
	for (int i = 0; i < argc; i++) {
		std::string arg = args[i];
		if (arg[0] == '-') {
			while (arg[0] == '-' && arg.size()) {
				arg = arg.substr(1);
			}
			m_args[arg] = true;
		}
	}
}

bool ClArgs::operator[](std::string arg) {
	return m_args[arg];
}

}
}
