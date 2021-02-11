/// Copyright (c) 2021 Shaye Garg.
/// \file
/// Entry point handler for cross-platform Unicode arguments.

#pragma once
#include "Core/Platform/Internals.h" // Need to include because we are receiving arguments from the OS.
#include "Core/Platform/Platform.h"
#include "Core/Types/Array.h"

/// Define this entry function.
int Entry(Ignis::StringRef invocation, Ignis::ArrayRef<Ignis::StringRef> arguments);

#ifdef PLATFORM_WINDOWS

int wmain(int argc, wchar_t** argv)
{
	using namespace Ignis;

	String invocation = PlatformInternals::ConvToUTF8(argv[0]);
	Array<String> argData;
	argData.Reserve(argc - 1);
	Array<StringRef> arguments;
	arguments.Reserve(argc - 1);
	for (int i = 1; i < argc; i++)
	{
		argData.Push(PlatformInternals::ConvToUTF8(argv[i]));
		arguments.Push(argData[i - 1]);
	}

	return Entry(invocation, arguments);
}

#else

int main(int argc, char** argv)
{
	StringRef invocation = argv[0];
	Array<StringRef> arguments(argc - 1);
	for (int i = 1; i < argc; i++)
	{
		arguments[i - 1] = argv[i];
	}

	return Entry(invocation, arguments);
}

#endif
