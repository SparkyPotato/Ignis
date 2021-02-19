/// Copyright (c) 2021 Shaye Garg.
/// \file
/// Pair.

#pragma once

namespace Ignis {

/// Structure for holding a pair of types.
/// 
/// \tparam F First type to hold.
/// \tparam S Second type to hold.
template<typename F, typename S>
struct Pair
{
	/// First element of the pair.
	F First;

	/// Second element of the pair.
	S Second;
};

}
