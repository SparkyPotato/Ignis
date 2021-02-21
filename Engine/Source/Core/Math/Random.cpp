/// Copyright (c) 2021 Shaye Garg.

#include "Core/Math/Random.h"

namespace Ignis {

u64 QuickRandom()
{
	// Uses Marsaglia's xorshift*

	static u64 state = 123456789;

	u64 x = state;
	x ^= x >> 12;
	x ^= x << 25;
	x ^= x >> 27;
	state = x;

	return x * 0x2545f4914f6cdd1d;
}

}
