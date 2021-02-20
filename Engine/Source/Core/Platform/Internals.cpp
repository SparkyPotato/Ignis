/// Copyright (c) 2021 Shaye Garg.

#include "Core/Platform/Internals.h"

#ifdef PLATFORM_WINDOWS
#	define WIN32_LEAN_AND_MEAN
#	include <Windows.h> // Yikes
#endif

namespace Ignis {
namespace PlatformInternals {

#ifdef PLATFORM_WINDOWS

String ConvToUTF8(wchar_t* utf16str)
{
	int length = WideCharToMultiByte(CP_UTF8, 0, utf16str, -1, nullptr, 0, nullptr, nullptr);
	String str(length);
	WideCharToMultiByte(
		CP_UTF8, 0, utf16str, -1, reinterpret_cast<char*>(str.Data()), int(str.Size() + 1), nullptr, nullptr);
	return str;
}

String ConvToUTF16(StringRef utf8str)
{
	int length = MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char*>(utf8str.Data()), -1, nullptr, 0);
	String str(length * sizeof(wchar_t));
	MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char*>(utf8str.Data()), -1,
		reinterpret_cast<wchar_t*>(str.Data()), int(str.Size() + 1));
	return str;
}

void DebugBreak() { __debugbreak(); }

void DebugOutput(StringRef message)
{
	OutputDebugStringW(reinterpret_cast<const wchar_t*>(ConvToUTF16(message).CString()));
}

#else

#	include <stdio.h>

void DebugBreak()
{
#	ifdef ARCH_X64
	__asm__ volatile("int $0x03");
#	elif defined(ARCH_ARM)
#		ifdef PLATFORM_MAC
	__builtin_trap();
#		else
	__asm__ volatile(".inst 0xd4200000");
#		endif
#	endif
}

void DebugOutput(StringRef message) { printf("%s", message.Data()); }

#endif

}
}
