/// Copyright (c) 2021 Shaye Garg.

#include "Core/Platform/Entry.h"
#include "Core/Job/JobSystem.h"

using namespace Ignis;

int Entry(StringRef invocation, ArrayRef<StringRef> arguments)
{
	JobSystem::Initialize();

	return 0;
}
