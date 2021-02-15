/// Copyright (c) 2021 Shaye Garg.
/// \file
/// Native OS mutexes.

#pragma once
#include "Core/Platform.h"

namespace Ignis {

class IGNIS_API Mutex
{
public:
	Mutex();

	Mutex(const Mutex& other) = delete;
	Mutex(Mutex&& other) = delete;

	/// Lock the mutex from the calling thread.
	/// Will deadlock if the thread already owns the mutex.
	void Lock();

	/// Try to lock the mutex and immediately return.
	///
	/// \return If the lock was acquired successfully.
	bool TryLock();

	/// Unlock the mutex.
	void Unlock();

private:
	void* m_PlatformHandle;
};

}
