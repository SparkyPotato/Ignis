/// Copyright (c) 2021 Shaye Garg.
/// \file
/// A Job to be submitted to the JobSystem. You are going to dote him.

#pragma once
#include "Core/Types/Any.h"
#include "Core/Types/Function.h"

namespace Ignis {

/// A job to be submitted to the job system.
struct IGNIS_API Job
{
	/// Function to run when the job is scheduled.
	/// The base callable object MUST survive till the job is completed.
	FunctionRef<void(AnyRef)> Func;

	/// Argument that is passed to Func.
	AnyRef Argument;

	/// Padding so that a job declaration occupies exactly 64 bytes.
	/// This is so that one declaration fits in a single cache line, 
	/// and there is no contention between cores for that cache line (false sharing).
	/// You can store the argument here if it fits - which is why we don't
	/// alignas(std::hardware_destructive_interference_size) - because that wouldn't let us use the padding storage.
	u8 Padding[64 - (sizeof(Func) + sizeof(Argument))];
};

static_assert(sizeof(Job) == 64, "Size of job must be 64 bytes! Adjust Padding.");

}
