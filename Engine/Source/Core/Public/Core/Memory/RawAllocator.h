/// Copyright (c) 2021 Shaye Garg.
/// \file 
/// Raw system memory allocator.

#pragma once
#include "Core/Memory/Allocator.h"

namespace Ignis {

/// Allocator that directly allocates system memory.
class RawAllocator : public Allocator
{
public:
	void* Allocate(u64 size) override;
	void Deallocate(void* ptr) override;
	u64 GrowAllocation(void* ptr, u64 oldSize, u64 newSize) override;
};

/// Global Raw Allocator for allocating memory directly on the heap.
extern RawAllocator GAlloc;

}
