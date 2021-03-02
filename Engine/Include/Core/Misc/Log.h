/// Copyright (c) 2021 Shaye Garg.
/// \file
/// Logging.

#include <cstdlib>

#include "Core/Misc/Format.h"
#include "Core/Types/Array.h"
#include "Core/Types/Pointer.h"
#include "Core/Types/Time.h"

namespace Ignis {

/// Level for a Log. From lowest to highest.
/// Fatal ends execution immediately after it is pushed to all sinks.
enum class LogLevel
{
	Verbose,
	Debug,
	Log,
	Warning,
	Error,
	Fatal
};

/// Sink for all log messages.
/// Must be thread safe.
class IGNIS_API LogSink
{
public:
	virtual ~LogSink() = default;

	virtual void Sink(LogLevel level, StringRef message) = 0;
};

/// Singleton for logging.
class IGNIS_API Logger
{
public:
	/// Get the singleton.
	///
	/// \return The Logger singleton.
	static Logger* Get();

	Logger(const Logger& other) = delete;

	/// Add a sink to the logger.
	///
	/// \param UniquePtr of the sink to move.
	void AddSink(UniquePtr<LogSink>&& sink);

	/// Log a message. Don't use, use the ILOG macro instead.
	template<typename... Args>
	void Log(StringRef category, LogLevel level, StringRef message, Args&&... args)
	{
		String fmtString = "[{:02d}:{:02d}:{:02d}:{:03d}][{}] {}: " + message;
		auto time = Time::Now();
		String log = Format(fmtString, time.Hour, time.Minute, time.Second, time.Millisecond, LevelToString(level),
			category, static_cast<Args&&>(args)...);
		for (auto& sink : m_Sinks)
		{
			sink->Sink(level, log);
		}

		if (level == LogLevel::Fatal)
		{
			std::exit(1);
		}
	}

private:
	Logger();

	StringRef LevelToString(LogLevel level);

	static Logger* s_Logger;

	Array<UniquePtr<LogSink>> m_Sinks;
};

/// A category for logging. Use the ILOG_CATEGORY macros below.
template<LogLevel CompileLevel>
struct LogCategory
{
public:
	LogCategory(StringRef name) : Name(name) {}

	template<LogLevel Level, typename... Args>
	void Log(StringRef message, Args&&... args)
	{
		if constexpr (Level >= CompileLevel)
		{
			Logger::Get()->Log(Name, Level, message, static_cast<Args&&>(args)...);
		}
	}

	StringRef Name;
};

class IGNIS_API StdoutSink : public LogSink
{
public:
	void Sink(LogLevel level, StringRef message) override;
};

}

/// Declare a LogCategory. Should be placed in a header file.
/// Can then be accessed by anyone who includes the header file.
///
/// \param name Name of the category.
/// \param level The level of the category to log. Anything under this will not be logged, and will be removed at
/// compile-time.
#define IDECLARE_LOG_CATEGORY(name, level) extern ::Ignis::LogCategory<LogLevel::level> LogCategory_##name;

/// Define a LogCategory declared with IDECLARE_LOG_CATEGORY(). For every IDECLARE_LOG_CATEGORY() in a header,
/// there must be a corresponding IDEFINE_LOG_CATEGORY() in a source.
///
/// \param name Name of the category.
/// \param level Compile-time level of the log category.
#define IDEFINE_LOG_CATEGORY(name, level) ::Ignis::LogCategory<LogLevel::level> LogCategory_##name(#name);

/// Declare and define a LOG_CATEGORY to be used in a source file.
///
/// \param name Name of the category.
/// \param level Level of the category.
#define ILOG_CATEGORY_LOCAL(name, level) static ::Ignis::LogCategory<LogLevel::level> LogCategory_##name(#name);

/// Log a message.
///
/// \param category Name of the category to log to.
/// \param level Level of the log.
/// \param ... Log message format string and format arguments.
#define ILOG(category, level, ...) ::LogCategory_##category.Log<LogLevel::level>(__VA_ARGS__)
