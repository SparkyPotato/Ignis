/// Copyright (c) 2021 Shaye Garg.

#include "Core/Platform/Entry.h"
#include "Core/Misc/Log.h"

using namespace Ignis;

ILOG_CATEGORY_LOCAL(LogEntry, Verbose);

int Entry(StringRef invocation, ArrayRef<StringRef> arguments)
{
	ILOG(LogEntry, Log, "Hello, {}", "world!");

	return 0;
}
