/// Copyright (c) 2021 Shaye Garg.
/// \file
/// Text formatting.

#pragma once
#include "fmt/chrono.h"
#include "fmt/format.h"

#include "Core/Types/String.h"
#include "Core/Types/Time.h"

namespace Ignis {

/// Format to a string. Uses fmt, so use their docs for format specifiers.
///
/// \param format Format string.
/// \param args Format arguments.
template<typename... Args>
String Format(StringRef format, Args&&... args)
{
	static fmt::memory_buffer out;
	fmt::format_to(out, reinterpret_cast<const char*>(format.Data()), static_cast<Args&&>(args)...);
	return out.data();
}

}

namespace fmt {

template<>
struct formatter<Ignis::String> : formatter<std::string_view>
{
	template<typename FormatContext>
	auto format(const Ignis::String& string, FormatContext& ctx)
	{
		return formatter<std::string_view>::format(
			std::string_view(reinterpret_cast<const char*>(string.CString()), string.Size()), ctx);
	}
};

template<>
struct formatter<Ignis::StringRef> : formatter<std::string_view>
{
	template<typename FormatContext>
	auto format(const Ignis::StringRef& string, FormatContext& ctx)
	{
		return formatter<std::string_view>::format(
			std::string_view(reinterpret_cast<const char*>(string.Data()), string.Size()), ctx);
	}
};

}
