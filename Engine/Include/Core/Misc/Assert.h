/// Copyright (c) 2021 Shaye Garg.
/// \file
/// Debug Asserts.

#pragma once
#include "Core/Types/String.h"

namespace Ignis {

void IGNIS_API Assert(bool condition, StringRef message);

}

#ifdef NDEBUG
#	define IASSERT(condition, message)
#else
#	define IASSERT(condition, message) Assert(condition, message)
#endif
