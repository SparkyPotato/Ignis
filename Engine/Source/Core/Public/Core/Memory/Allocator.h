/// Copyright (c) 2021 Shaye Garg.
/// \file 
/// Memory allocator interface.

#pragma once
#include "Core/Types/BaseTypes.h"

namespace Ignis {

/// Interface for all memory allocators.
/// Every byte of heap memory MUST come from one of these.
class Allocator
{
public:
	virtual ~Allocator() = default;

	/// Allocate memory with 16 byte alignment.
	///
	/// \param size The number of bytes to allocate.
	///
	/// \return Pointer to the first byte allocated.
	virtual void* Allocate(u64 size) = 0;

	/// Deallocate memory allocated by the allocator.
	///
	/// \param ptr The pointer returned from Allocate.
	virtual void Deallocate(void* ptr) = 0;

	/// Grow an allocation in place.
	///
	/// \param ptr The pointer returned from Allocate.
	/// \param oldSize The old size of the allocation.
	/// \param newSize The new size of the allocation requested.
	///
	/// \return The number of bytes usable after growth. May not be equal to newSize, and may be equal to oldSize.
	virtual u64 GrowAllocation(void* ptr, u64 oldSize, u64 newSize) = 0;
};

}
