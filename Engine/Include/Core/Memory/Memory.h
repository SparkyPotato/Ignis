/// Copyright (c) 2021 Shaye Garg.
/// \file
/// Memory related helper functions.

#pragma once
#include <new> // Because self-defining placement new doesn't work with MSVC. How lovely.

#include "Core/Memory/RawAllocator.h"

namespace Ignis {

/// Copy from one location to another.
///
/// \param destination Destination for the copy.
/// \param source Source for the copy.
/// \param size Number of bytes to copy.
void IGNIS_API MemCopy(void* destination, const void* source, u64 size);

/// Set all bytes to a specific value.
///
/// \param destination Destination for the set.
/// \param value Value to set each byte.
/// \param size Number of bytes to set.
void IGNIS_API MemSet(void* destination, Byte value, u64 size);

/// Compare two regions of memory.
///
/// \param first First region of memory.
/// \param second Second region of memory.
/// \param size Number of bytes to compare.
///
/// \return If both regions of memory are equal.
bool IGNIS_API MemCompare(const void* first, const void* second, u64 size);

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
/// \return Pointer to the constructed object.
template<typename T, typename... Args>
T* Construct(void* at, Args&&... args)
{
	return new (at) T(static_cast<Args&&>(args)...);
}

/// Allocate memory for an object from an allocator, and construct it.
///
/// \tparam T Type of object to construct.
/// \param alloc Allocator to use.
/// \param args Arguments to pass to the constructor.
///
/// \return Pointer to the constructed object.
template<typename T, typename... Args>
T* New(Allocator& alloc, Args&&... args)
{
	auto ptr = alloc.Allocate(sizeof(Allocator*) + sizeof(T));
	ptr = &alloc;
	return Construct<T>(reinterpret_cast<Allocator*>(ptr) + 1, static_cast<Args&&>(args)...);
}

/// Delete an object allocated with New<T>().
///
/// \tparam T Type of the object.
/// \param object Object to delete. MUST have been New<T>()ed.
template<typename T>
void Delete(T* object)
{
	object->~T();
	auto ptr = reinterpret_cast<Allocator*>(object) - 1;
	ptr->Deallocate(ptr + 1);
}

}
