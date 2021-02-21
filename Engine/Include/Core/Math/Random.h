/// Copyright (c) 2021 Shaye Garg.
/// \file
/// Random number generators.

#pragma once
#include "Core/Types/BaseTypes.h"

namespace Ignis {

/// Quickly generate a random number. Isn't very high quality.
/// Has a period of 2^64 - 1.
/// 
/// \return A 64-bit random integer.
u64 IGNIS_API QuickRandom();

}
