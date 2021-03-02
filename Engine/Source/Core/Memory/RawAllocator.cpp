/// Copyright (c) 2021 Shaye Garg.

#include "Core/Memory/RawAllocator.h"

#include <cstdlib>

namespace Ignis {

/// Ensuring that GAlloc is constructed before anything else and destroyed last.
#ifdef COMPILER_MSVC
#	pragma init_seg(lib)
RawAllocator GAlloc;
#else
RawAllocator GAlloc __attribute__((init_priority(101)));
#endif

void* RawAllocator::Allocate(u64 size) { return malloc(size); }

void RawAllocator::Deallocate(void* ptr) { free(ptr); }

u64 RawAllocator::GrowAllocation(void* ptr, u64 oldSize, u64 newSize) { return oldSize; }

}
