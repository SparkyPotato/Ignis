/// Copyright (c) 2021 Shaye Garg.

#include "Platform/Entry.h"
#include "Job/JobSystem.h"

#include <stdio.h>

using namespace Ignis;

int Entry(StringRef invocation, ArrayRef<StringRef> arguments) 
{
	JobSystem::Initialize(0);

	const WaitCondition* cond;

	{
		Job jobs[10];
		auto callable = Bind<void, AnyRef>([](AnyRef) { printf("Hello World!\n"); });

		jobs[0].Func = callable;
		jobs[1].Func = callable;
		jobs[2].Func = callable;
		jobs[3].Func = callable;
		jobs[4].Func = callable;
		jobs[5].Func = callable;
		jobs[6].Func = callable;
		jobs[7].Func = callable;
		jobs[8].Func = callable;
		jobs[9].Func = callable;

		cond = &JobSystem::SubmitAsync(ArrayRef<Job>(jobs, 10));
	}

	JobSystem::Wait(*cond);

	JobSystem::Quit();

	return 0; 
}
