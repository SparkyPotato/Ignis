/// Copyright (c) 2021 Shaye Garg.
/// \file
/// Helpers for concepts and traits.

#pragma once
#include "Core/Types/BaseTypes.h"

namespace Ignis {

namespace Traits {

/// Compile-time constant template.
template<typename T, T Val>
struct Constant
{
	/// Value of the Constant.
	static constexpr T Value = Val;

	constexpr operator T() { return Value; }
	constexpr T operator()() { return Value; }
};

/// If two types are the same.
///
/// \tparam T first type.
/// \tparam U second type.
template<typename T, typename U>
struct IsSame : Constant<bool, false>
{
};

/// If two types are the same.
///
/// \tparam T first type.
template<typename T>
struct IsSame<T, T> : Constant<bool, true>
{
};

/// Concept for IsSame.
template<typename T, typename U>
concept SameAs = IsSame<T, U>::Value;

/// Helper for IsComplete.
template<typename T, u64 = sizeof(T)>
Constant<bool, true> IsCompleteHelper(T*); // Undefined because only used in decltype.
Constant<bool, false> IsCompleteHelper(void*); // Undefined because only used in decltype.

/// Checking if a type is complete.
/// Is Constant<bool, true> if the type is complete.
/// Is Constant<bool, false> if type is incomplete.
///
/// \tparam T Type to check.
template<typename T>
using IsComplete = decltype(IsCompleteHelper((T*) nullptr));

}

}
