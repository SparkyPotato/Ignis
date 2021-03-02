/// Copyright (c) 2021 Shaye Garg.

#include "Core/Job/JobSystem.h"

#include <atomic>
#include <thread>

#include "Core/Misc/Log.h"
#include "Core/Platform/Thread.h"

namespace Ignis {

namespace JobSystem {

ILOG_CATEGORY_LOCAL(LogJobSystem, Verbose);

static std::atomic_flag s_Initialized;
static Array<Thread> s_Threads;

class Counter : public WaitCondition
{
public:
	Counter() = default;
	Counter(const Counter& other) : Count(other.Count.load()) {}

	operator bool() const override { return Count == 0; }

	void SleepOn() const override
	{
		while (Count > 0) {} // Busy wait :sob:
	}

	std::atomic<u64> Count = 0;
};

static void ThreadFunction() { 
	ILOG(LogJobSystem, Verbose, "Job System worker thread started"); 
}

void Initialize(u16 threadCount, u64 memUsage)
{
	if (s_Initialized.test_and_set())
	{
		ILOG(LogJobSystem, Error, "Job System has already been initialized! Doing nothing");
		return;
	}

	if (!threadCount)
	{
		threadCount = Thread::GetMaxThreads() - 1;
	}
	else if (threadCount > Thread::GetMaxThreads() * 2)
	{
		ILOG(LogJobSystem, Warning,
			"threadCount ({}) is more than double the maximum amount of concurrent threads supported by the hardware "
			"({}), performance may be greatly reduced",
			threadCount, Thread::GetMaxThreads());
	}

	ILOG(
		LogJobSystem, Verbose, "Initializing Job System with {} threads, using {} MB of memory", threadCount, memUsage);

	s_Threads.Reserve(threadCount);
	for (u64 i = 0; i < threadCount; i++)
	{
		s_Threads.Emplace(&ThreadFunction);
	}
}

}

}
