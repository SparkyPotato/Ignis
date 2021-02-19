/// Copyright (c) 2021 Shaye Garg.

#pragma once
#include <immintrin.h>

#include "Core/Types/BaseTypes.h"

namespace Ignis {

#ifdef PLATFORM_WINDOWS

/// Callee preserved registers on Microsoft ABI.
/// DO NOT change without editing all ___MS.asm.
struct FiberContext
{
	void* rip = nullptr;
	void* rsp = nullptr;
	u64 rbx = 0, rbp = 0, r12 = 0, r13 = 0, r14 = 0, r15 = 0, rdi = 0, rsi = 0;
	__m128i xmm6, xmm7, xmm8, xmm9, xmm10, xmm11, xmm12, xmm13, xmm14, xmm15;
};

#else

/// Callee preserved registers on SysV ABI.
/// DO NOT change without editing all ___SysV.asm.
struct FiberContext
{
	void* rip = nullptr;
	void* rsp = nullptr;
	u64 rbx = 0, rbp = 0, r12 = 0, r13 = 0, r14 = 0, r15 = 0;
};

#endif

/// Set the current thread's context.
///
/// \param from Context to swap from.
/// \param to Context to swap to.
extern void (*SwapContext)(FiberContext* from, FiberContext* to);

}
