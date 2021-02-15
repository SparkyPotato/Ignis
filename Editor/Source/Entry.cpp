/// Copyright (c) 2021 Shaye Garg.

#include "Platform/Entry.h"
#include "Job/JobSystem.h"

using namespace Ignis;

int Entry(StringRef invocation, ArrayRef<StringRef> arguments) 
{
	JobSystem::Initialize(0, 256);

	JobSystem::Quit();

	return 0; 
}
