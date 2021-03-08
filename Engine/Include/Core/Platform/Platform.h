/// Copyright (c) 2021 Shaye Garg.
/// \file
/// Platform detection macros for conditional compilation.

#pragma once

// OS detection
#ifdef _WIN32
#	ifdef _WIN64
#		define PLATFORM_WINDOWS
#	else
#		error 32-bit Windows is not supported!
#	endif
#elif defined(__APPLE__) && defined(__MACH__)
#	include <TargetConditionals.h>
#	if TARGET_OS_MAC == 1
#		define PLATFORM_MAC
#	else
#		error macOS is the only supported Apple platform!
#	endif
#elif defined(__linux__)
#	define PLATFORM_LINUX
#else
#	error Unknown OS!
#endif

// Compiler and architecture detection
#ifdef _MSC_VER
#	define COMPILER_MSVC
#	ifdef _M_X64
#		define ARCH_X64
#	elif _M_ARM64
#		define ARCH_ARM
#	else
#		error Unsupported CPU architecture!
#	endif
#elif __clang__
#	define COMPILER_CLANG
#	if __x86_64 == 1
#		define ARCH_X64
#	elif __aarch64__
#		define ARCH_ARM
#	else
#		error Unsupported CPU architecture!
#	endif
#elif __GNUC__
#	define COMPILER_GCC
#	ifdef __x86_64__
#		define ARCH_X64
#	elif __aarch64__
#		define ARCH_ARM
#	else
#		error Unsupported CPU architecture!
#	endif
#else
#	error Unsupported compiler!
#endif

#ifdef PLATFORM_WINDOWS
#	ifdef Ignis_EXPORTS
#		define IGNIS_API __declspec(dllexport)
#	else
#		define IGNIS_API __declspec(dllimport)
#	endif
#else
#	define IGNIS_API
#endif
