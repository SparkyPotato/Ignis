/// Copyright (c) 2021 Shaye Garg.
/// \file 
/// Internal functions used while interacting with the OS.
/// Don't include unless you're actually interacting with the OS.

#pragma once

#include "Core/Platform/Platform.h"
#include "Core/Types/String.h"

namespace Ignis {
namespace PlatformInternals {

#ifdef PLATFORM_WINDOWS

/// Convert a UTF-16 string to UTF-8.
///
/// \param utf16str Null-terminated UTF-16 encoded string.
///
/// \return UTF-8 encoded String for use throughout the Engine.
String ConvToUTF8(wchar_t* utf16str);

/// Convert a UTF-8 string to UTF-16
///
/// \param utf8str String to convert.
/// 
/// \return UTF-16 encoded String to directly call Win32 API functions with.
/// DO NOT use any methods other than Data() on it.
/// THIS IS THE ONLY PLACE WHERE A String IS NOT ENCODED IN UTF-8.
String ConvToUTF16(StringRef utf8str);

#endif

}
}
