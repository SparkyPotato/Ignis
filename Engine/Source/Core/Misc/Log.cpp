/// Copyright (c) 2021 Shaye Garg.

#include "Core/Misc/Log.h"

#include <cstdio>

#include "fmt/color.h"
#include "fmt/core.h"

#include "Core/Misc/Format.h"

namespace Ignis {

Logger* Logger::s_Logger = nullptr;

Logger::Logger() { m_Sinks.Push(MakeUnique<StdoutSink>()); }

StringRef Logger::LevelToString(LogLevel level)
{
	switch (level)
	{
	case LogLevel::Verbose:
		return "Verbose";
	case LogLevel::Debug:
		return "Debug";
	case LogLevel::Log:
		return "Log";
	case LogLevel::Warning:
		return "Warning";
	case LogLevel::Error:
		return "Error";
	case LogLevel::Fatal:
		return "Fatal";
	}

	return "";
}

Logger* Logger::Get()
{
	if (!s_Logger)
	{
		s_Logger = new Logger;
	}

	return s_Logger;
}

void Logger::AddSink(UniquePtr<LogSink>&& sink) { m_Sinks.Emplace(std::move(sink)); }

void StdoutSink::Sink(LogLevel level, StringRef message)
{
	switch (level)
	{
	case LogLevel::Verbose:
		return fmt::print(fg(fmt::color::gray), "{}\n", message);
	case LogLevel::Debug:
		return fmt::print(fg(fmt::color::light_cyan), "{}\n", message);
	case LogLevel::Log:
		return fmt::print(fg(fmt::color::light_green), "{}\n", message);
	case LogLevel::Warning:
		return fmt::print(fg(fmt::color::yellow), "{}\n", message);
	case LogLevel::Error:
		return fmt::print(fg(fmt::color::red), "{}\n", message);
	case LogLevel::Fatal:
		return fmt::print(fg(fmt::color::dark_red), "{}\n", message);
	}
}

}
