/// Copyright (c) 2021 Shaye Garg.
/// \file
/// Entry handler for Ignis applications.

#pragma once
#include "Core/Types/Array.h"
#include "Core/Types/String.h"

/// Define this entry point.
///
/// \param invocation The invocation string of the executable.
/// \param arguments Command-line arguments.
///
/// \return Return value, 0 on success.
int Entry(Ignis::StringRef invocation, Ignis::ArrayRef<Ignis::StringRef> arguments);

#ifdef PLATFORM_WINDOWS

#	include "Core/Platform/Internals.h"

int wmain(int argc, wchar_t** argv)
{
	using namespace Ignis;

	String invocation = PlatformInternals::ConvToUTF8(argv[0]);

	Array<String> argData;
	Array<StringRef> args;
	argData.Reserve(argc - 1);
	args.Reserve(argc - 1);
	for (int i = 1; i < argc; i++)
	{
		args.Push(argData.Emplace(PlatformInternals::ConvToUTF8(argv[i])));
	}

	return Entry(invocation, args);
}

#else

int main(int argc, char** argv) 
{
	using namespace Ignis;

	StringRef invocation = argv[0];

	Array<StringRef> args;
	args.Reserve(argc - 1);
	for (int i = 1; i < argc; i++)
	{
		args.Push(argv[i]);
	}

	return Entry(invocation, args);
}

#endif
