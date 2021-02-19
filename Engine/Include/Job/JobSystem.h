/// Copyright (c) 2021 Shaye Garg.
/// \file
/// Ignis Job System. He's your best buddy.

#pragma once
#include "Core/Types/Array.h"
#include "Job/Condition.h"
#include "Job/Job.h"

namespace Ignis {

namespace JobSystem {

/// Initialize the job system, with the current thread as the main thread.
///
/// \param threadCount Number of worker threads to spawn.
/// If 0, is set to the maximum number of concurrent threads supported by the hardware - 1.
/// \param memUsage Memory to use for Fiber stacks, in MB. Defaults to 100 MB.
IGNIS_API void Initialize(u16 threadCount, u64 memUsage = 100);

/// Submit a list of jobs to the job system.
/// Use this when you're submitting jobs and doing some more work before waiting for them to complete.
///
/// \param jobs Reference to the list of jobs to submit.
/// Must survive until all jobs have finished execution, as no copies are made.
/// 
/// \return Condition to wait on for the jobs to complete.
IGNIS_API const WaitCondition& Submit(ArrayRef<Job> jobs);

/// Submit a list of jobs to the job system.
/// Use this when you're submitting jobs and don't care when they complete.
///
/// \param jobs Reference to the list of jobs to submit.
/// Job declarations are copied to the fiber stack, so they do not have to survive until they finish executing.
/// 
/// \return Condition to wait on for the jobs to complete.
IGNIS_API const WaitCondition& SubmitAsync(ArrayRef<Job> jobs);

/// Submit a list of jobs to the job system, and pause the current job till they complete.
///
/// \param jobs Reference to the list of jobs to submit.
IGNIS_API void SubmitAndWait(ArrayRef<Job> jobs);

/// Pause the current job until a condition becomes true.
///
/// \param condition The condition to wait for.
IGNIS_API void Wait(const WaitCondition& condition);

/// Force the JobSystem to immediately terminate all worker threads.
IGNIS_API void Quit();

}

}
