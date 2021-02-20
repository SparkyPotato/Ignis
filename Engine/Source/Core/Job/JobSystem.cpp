/// Copyright (c) 2021 Shaye Garg.

#include "Core/Job/JobSystem.h"

#include <atomic>

#include "Core/Types/Array.h"
#include "Core/Types/Pair.h"
#include "Core/Types/Queue.h"
#include "Core/Job/Fiber.h"
#include "cORE/Platform/Threading/Thread.h"

namespace Ignis {

namespace JobSystem {

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

static void FiberFunction();

struct Fiber
{
	Fiber() { Context.rip = reinterpret_cast<void*>(&FiberFunction); }

	FiberContext Context;
	u8 Stack[64 * 1024];
};

struct RunJob
{
	const Job* Declaration;
	Fiber* Context;
	Counter* Decrement = nullptr;
};

static Array<Thread> s_Threads;
static Array<Fiber> s_Fibers;
static Array<Counter> s_Counters;

static MPMCQueue<RunJob> s_JobQueue;
static MPMCQueue<Fiber*> s_FreeFibers;
static MPMCQueue<Counter*> s_FreeCounters;

static thread_local FiberContext s_ThreadContext;
static thread_local FiberContext* s_SwitchTo = nullptr;
static thread_local RunJob s_Job;

void FiberFunction()
{
	s_Job.Declaration->Func(s_Job.Declaration->Argument);
	SwapContext(&s_Job.Context->Context, s_SwitchTo);
}

static void ThreadFunction()
{
	while (true)
	{
		while (s_JobQueue.TryPop(s_Job))
		{
			s_SwitchTo = &s_ThreadContext;
			SwapContext(&s_ThreadContext, &s_Job.Context->Context);

			s_FreeFibers.Push(s_Job.Context);

			s_Job.Decrement->Count--;

			if (*s_Job.Decrement)
			{
				s_FreeCounters.Push(s_Job.Decrement);
			}
		}
	}
}

void Initialize(u16 threadCount, u64 memUsage)
{
	if (threadCount == 0)
	{
		threadCount = Thread::GetMaxThreads() - 1;
	}

	u64 fiberCount = memUsage * 16;

	s_JobQueue = MPMCQueue<RunJob>(fiberCount);
	s_FreeFibers = MPMCQueue<Fiber*>(fiberCount);
	s_FreeCounters = MPMCQueue<Counter*>(fiberCount);

	s_Fibers.Reserve(fiberCount);
	s_Counters.Reserve(fiberCount);
	for (u64 i = 0; i < fiberCount; i++)
	{
		s_Fibers.Emplace();
		s_Counters.Emplace();
		s_FreeFibers.Push(s_Fibers.Data() + i);
		s_FreeCounters.Push(s_Counters.Data() + i);
	}

	s_Threads.Reserve(threadCount);
	for (u16 i = 1; i <= threadCount; i++)
	{
		auto& thread = s_Threads.Emplace(&ThreadFunction);
		thread.SetName(String("Thread ").Push(i + 48));
	}
}

const WaitCondition& Submit(ArrayRef<Job> jobs)
{
	Counter* counter = s_FreeCounters.Pop();
	counter->Count = jobs.Size();

	for (auto& job : jobs)
	{
		Fiber* fiber = s_FreeFibers.Pop();
		fiber->Context.rsp = fiber->Stack + sizeof(fiber->Stack);

		RunJob runJob;
		runJob.Declaration = &job;
		runJob.Context = fiber;
		runJob.Decrement = counter;

		s_JobQueue.Push(runJob);
	}

	return *counter;
}

class CloneAllocator : public Allocator
{
public:
	CloneAllocator(u8*& end) : m_End(end) {}

	void* Allocate(u64 size) override
	{
		m_End -= (size & ~(u64(-1) & 0b1111)) + 16; // Round to next multiple of 16
		return m_End;
	}

	void Deallocate(void* ptr) override {}

	u64 GrowAllocation(void* ptr, u64 oldSize, u64 newSize) override { return oldSize; }

private:
	u8*& m_End;
};

static Job* SetupAsyncStack(Fiber* fiber, const Job& job)
{
	u8* rsp = fiber->Stack + sizeof(fiber->Stack);
	rsp -= sizeof(Job);
	Job* ret = Construct<Job>(rsp, job);
	CloneAllocator alloc(rsp);
	ret->Func = *ret->Func.GetCallable()->Clone(nullptr, 0, alloc);
	
	fiber->Context.rsp = rsp;

	return ret;
}

const WaitCondition& SubmitAsync(ArrayRef<Job> jobs)
{
	Counter* counter = s_FreeCounters.Pop();
	counter->Count = jobs.Size();

	for (auto& job : jobs)
	{
		Fiber* fiber = s_FreeFibers.Pop();

		RunJob runJob;
		runJob.Declaration = SetupAsyncStack(fiber, job);
		runJob.Context = fiber;
		runJob.Decrement = counter;

		s_JobQueue.Push(runJob);
	}

	return *counter;
}

void SubmitAndWait(ArrayRef<Job> jobs)
{
	Counter* counter = s_FreeCounters.Pop();
	counter->Count = jobs.Size();

	for (auto& job : jobs)
	{
		Fiber* fiber = s_FreeFibers.Pop();
		fiber->Context.rsp = fiber->Stack + sizeof(fiber->Stack);

		RunJob runJob;
		runJob.Declaration = &job;
		runJob.Context = fiber;
		runJob.Decrement = counter;

		s_JobQueue.Push(runJob);
	}

	while (!*counter)
	{
		if (!s_JobQueue.TryPop(s_Job))
		{
			continue;
		}

		s_SwitchTo = &s_ThreadContext;
		SwapContext(&s_ThreadContext, &s_Job.Context->Context);

		s_FreeFibers.Push(s_Job.Context);

		s_Job.Decrement->Count--;
	}

	s_FreeCounters.Push(counter);
}

void Wait(const WaitCondition& condition)
{
	while (!condition)
	{
		if (!s_JobQueue.TryPop(s_Job))
		{
			continue;
			;
		}

		s_SwitchTo = &s_ThreadContext;
		SwapContext(&s_ThreadContext, &s_Job.Context->Context);

		s_FreeFibers.Push(s_Job.Context);

		s_Job.Decrement->Count--;

		if (*s_Job.Decrement)
		{
			s_FreeCounters.Push(s_Job.Decrement);
		}
	}
}

void Quit() { s_Threads.Clear(); }

}

}
