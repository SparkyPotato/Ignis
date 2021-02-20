/// Copyright (c) 2021 Shaye Garg.

#include "Core/Platform/Entry.h"
#include "Core/Job/JobSystem.h"

#include "Core/Reflection/Resolver.h"

using namespace Ignis;

int Entry(StringRef invocation, ArrayRef<StringRef> arguments) 
{
	JobSystem::Initialize();

	JobSystem::Quit();

	return 0; 
}
