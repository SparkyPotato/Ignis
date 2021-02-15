/// Copyright (c) 2021 Shaye Garg.

#include "Job/JobSystem.h"

#include "Core/Misc/Assert.h"
#include "Core/Types/Array.h"
#include "Platform/Threading/Thread.h"

namespace Ignis {

namespace JobSystem {

static thread_local u16 s_ThreadIndex = 0;

static Array<Thread> s_WorkerThreads;

static void ThreadFunction(u16 index)
{
	s_ThreadIndex = index;
}

void Initialize(u16 threadCount, u16 fiberCount, u64 stackSize)
{
	IASSERT((fiberCount & (fiberCount - 1)) == 0, "fiberCount is not a power of two!");

	if (threadCount == 0)
	{
		threadCount = Thread::GetMaxThreads() - 1;
	}

	s_WorkerThreads.Reserve(threadCount);
	for (u16 i = 1; i <= threadCount; i++)
	{
		auto& thread = s_WorkerThreads.Emplace([i]() { ThreadFunction(i); });
		thread.SetName(String("Thread ").Push(i + 48));
	}
}

void Quit()
{
	s_WorkerThreads.Clear();
}

}

}
