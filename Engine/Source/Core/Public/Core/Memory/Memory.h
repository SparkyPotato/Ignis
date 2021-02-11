/// Copyright (c) 2021 Shaye Garg.
/// \file
/// Memory related helper functions.

#pragma once
#include <new> // Because self-defining placement new doesn't work with MSVC. How lovely.

#include "Core/Types/BaseTypes.h"

namespace Ignis {

/// Copy from one location to another.
///
/// \param destination Destination for the copy.
/// \param source Source for the copy.
/// \param size Number of bytes to copy.
void MemCopy(void* destination, const void* source, u64 size);

/// Set all bytes to a specific value.
///
/// \param destination Destination for the set.
/// \param value Value to set each byte.
/// \param size Number of bytes to set.
void MemSet(void* destination, Byte value, u64 size);

/// Compare two regions of memory.
///
/// \param first First region of memory.
/// \param second Second region of memory.
/// \param size Number of bytes to compare.
///
/// \return If both regions of memory are equal.
bool MemCompare(const void* first, const void* second, u64 size);

/// Convert an object into an r-value reference.
///
/// \param object Object to convert.
///
/// \return R-value reference to object.
template<typename T>
T&& Move(T&& object)
{
	return static_cast<T&&>(object);
}

/// Construct an object at a memory location.
///
/// \tparam T Type of object to construct.
/// \param at Location to construct the object.
/// \param args Arguments to pass to the constructor.
///
/// \return Reference to the constructed object.
template<typename T, typename... Args>
T* Construct(void* at, Args&&... args)
{
	return new (at) T(static_cast<Args&&>(args)...);
}

}
