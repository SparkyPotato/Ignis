/// Copyright (c) 2021 Shaye Garg.
/// \file
/// Base integral and character types.

#pragma once

#include <cstdint>

namespace Ignis {

/// Signed 8-bit integer.
using i8 = int8_t;
/// Signed 16-bit integer.
using i16 = int16_t;
/// Signed 32-bit integer.
using i32 = int32_t;
/// Signed 64-bit integer.
using i64 = int64_t;

/// Unsigned 8-bit integer.
using u8 = uint8_t;
/// Unsigned 16-bit integer.
using u16 = uint16_t;
/// Unsigned 32-bit integer.
using u32 = uint32_t;
/// Unsigned 64-bit integer.
using u64 = uint64_t;

/// 32-bit floating point number.
using f32 = float;
/// 64-bit floating point number.
using f64 = double;

/// UTF-32 encoded character.
struct Char
{
	Char(u32 data) : Data(data) {}

	operator u32() { return Data; }

	u32 Data;
};

/// A single byte.
struct Byte
{
	template<typename T>
	Byte(T data) : Data(u8(data))
	{
	}

	operator u8() const { return Data; }
	operator Char() const { return Data; }

	u8 operator&(int val) const { return Data & val; }

	u8 Data;
};

}
