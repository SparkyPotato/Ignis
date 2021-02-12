/// Copyright (c) 2021 Shaye Garg.

#include "Core/Threads/Atomic.h"

#include "Core/Platform/Platform.h"

#ifdef PLATFORM_WINDOWS

#	include <Windows.h>

namespace Ignis {

u64 Atomic<u64>::operator+=(u64 value) { return InterlockedAdd64((LONG64*) &m_Value, value); }

u64 Atomic<u64>::operator-=(u64 value) { return InterlockedAdd64((LONG64*) &m_Value, -i64(value)); }

u64 Atomic<u64>::operator&=(u64 value) { return InterlockedAnd64((LONG64*) &m_Value, value); }

u64 Atomic<u64>::operator|=(u64 value) { return InterlockedOr64((LONG64*) &m_Value, value); }

u64 Atomic<u64>::operator^=(u64 value) { return InterlockedXor64((LONG64*) &m_Value, value); }

u64 Atomic<u64>::operator++() { return InterlockedIncrement64((LONG64*) &m_Value); }

u64 Atomic<u64>::operator--() { return InterlockedDecrement64((LONG64*) &m_Value); }

u64 Atomic<u64>::CompareExchange(u64 exchange, u64 compare)
{
	return InterlockedCompareExchange64((LONG64*) &m_Value, exchange, compare);
}

u64 Atomic<u64>::operator=(u64 value)
{
	InterlockedExchange64((LONG64*) &m_Value, value);
	return value;
}

void MemoryBlock() { MemoryBarrier(); }

}

#endif
