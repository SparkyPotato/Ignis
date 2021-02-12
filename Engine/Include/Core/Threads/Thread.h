/// Copyright (c) 2021 Shaye Garg.
/// \file
/// Native OS threads.

#pragma once
#include "Core/Types/Function.h"
#include "Core/Types/String.h"

namespace Ignis {

/// Native thread object.
class IGNIS_API Thread
{
public:
	Thread() = default;

	/// Create a thread, and immediately starts it.
	///
	/// \param threadFunction Function to run in the thread.
	/// Callable object should last long enough for thread to start.
	Thread(FunctionRef<void()> threadFunction);

	~Thread();

	/// Put the calling thread to sleep until the thread has exited.
	void Join();

	/// Detach the thread from the handle and let it run.
	void Detach();

	/// Set the name of the thread.
	///
	/// \param name The name of the thread.
	void SetName(StringRef name);

	static u16 GetMaxThreads();

private:
	void* m_PlatformHandle = nullptr;
	u64 m_ID = 0;
};

}
