/// Copyright (c) 2021 Shaye Garg.
/// \file
/// Ignis Job System. He's your best buddy.

#pragma once
#include "Core/Types/BaseTypes.h"

namespace Ignis {

namespace JobSystem {

/// Initialize the job system, with the current thread as the main thread.
///
/// \param threadCount Number of worker threads to spawn. 
/// If 0, is set to the maximum number of concurrent threads supported by the hardware - 1.
/// \param fiberCount Number of fibers to spawn. MUST be a power of two.
/// \param stackSize for each fiber, in bytes. Defaults to 512KB.
void IGNIS_API Initialize(u16 threadCount, u16 fiberCount, u64 stackSize = 512 * 1024);

/// Force the JobSystem to immediately terminate all worker threads.
void IGNIS_API Quit();

}

}
