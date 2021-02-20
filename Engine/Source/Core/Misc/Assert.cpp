/// Copyright (c) 2021 Shaye Garg.

#include "Core/Misc/Assert.h"
#include "Core/Platform/Internals.h"

namespace Ignis {

void Assert(bool condition, StringRef message)
{
	if (condition)
	{
		return;
	}

	String output = "Assertion failed: " + message + "\n";
	PlatformInternals::DebugOutput(output);

	PlatformInternals::DebugBreak();
}

}
