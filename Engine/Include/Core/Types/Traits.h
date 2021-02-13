/// Copyright (c) 2021 Shaye Garg.
/// \file
/// Helpers for concepts and traits.

#pragma once
#include <type_traits>

#include "Core/Types/BaseTypes.h"

namespace Ignis {

namespace Traits {

/// Helper for IsComplete.
template<typename T, u64 = sizeof(T)>
std::bool_constant<true> IsCompleteHelper(T*);	   // Undefined because only used in decltype.
std::bool_constant<false> IsCompleteHelper(void*); // Undefined because only used in decltype.

/// Checking if a type is complete.
/// Is Constant<bool, true> if the type is complete.
/// Is Constant<bool, false> if type is incomplete.
///
/// \tparam T Type to check.
template<typename T>
using IsComplete = decltype(IsCompleteHelper((T*) nullptr));

}

}
