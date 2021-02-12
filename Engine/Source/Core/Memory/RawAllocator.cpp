/// Copyright (c) 2021 Shaye Garg.

#include "Core/Memory/RawAllocator.h"

#include <cstdlib>

namespace Ignis {

RawAllocator GAlloc;

void* RawAllocator::Allocate(u64 size) { return malloc(size); }

void RawAllocator::Deallocate(void* ptr) { free(ptr); }

u64 RawAllocator::GrowAllocation(void* ptr, u64 oldSize, u64 newSize) { return oldSize; }

}
