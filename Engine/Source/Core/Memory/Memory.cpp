/// Copyright (c) 2021 Shaye Garg.

#include "Core/Memory/Memory.h"

#include <cstring>

namespace Ignis {

void MemCopy(void* destination, const void* source, u64 size) { memcpy(destination, source, size); }

void MemSet(void* destination, Byte value, u64 size) { memset(destination, value, size); }

bool MemCompare(const void* first, const void* second, u64 size) { return memcmp(first, second, size) == 0; }

}
