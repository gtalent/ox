/*
 * Copyright 2015 - 2017 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "bitops.hpp"
#include "random.hpp"

namespace ox {

RandomSeed Random::DEFAULT_SEED = {540932923848, 540932540932};

Random::Random(RandomSeed seed) {
	m_seed[0] = seed[0];
	m_seed[1] = seed[1];
}

uint64_t Random::gen() {
	auto s0 = m_seed[0];
	auto s1 = m_seed[1];
	auto retval = s0 + s1;

	s1 ^= s0;
	m_seed[0] = ox::rotateLeft(s0, 55) ^ s1 ^ (s1 << 14);
	m_seed[1] = ox::rotateLeft(s1, 36);

	return retval;
}

}


uint64_t ox_rand() {
	static ox::Random rand;
	return rand.gen();
}
