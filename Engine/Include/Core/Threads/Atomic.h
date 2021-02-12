/// Copyright (c) 2021 Shaye Garg.
/// \file
/// Atomic operations.

#pragma once
#include "Core/Types/BaseTypes.h"

namespace Ignis {

/// Atomic types.
///
/// \tparam Type.
template<typename T>
class Atomic;

/// Atomic unsigned 64-bit integer.
template<>
class IGNIS_API Atomic<u64>
{
public:
	Atomic() = default;
	Atomic(u64 value) : m_Value(value) {}

	u64 operator=(u64 value);

	u64 operator+=(u64 value);
	u64 operator-=(u64 value);
	u64 operator&=(u64 value);
	u64 operator|=(u64 value);
	u64 operator^=(u64 value);

	u64 operator++();
	u64 operator--();

	operator u64() { return m_Value; }

	/// Compare with a value, and set if they are equal.
	///
	/// \param exchange The value to set.
	/// \param compare Value to compare with.
	/// 
	/// \return Initial value.
	u64 CompareExchange(u64 exchange, u64 compare);

private:
	u64 m_Value = 0;
};

/// Insert a memory barrier.
void IGNIS_API MemoryBlock();

}
