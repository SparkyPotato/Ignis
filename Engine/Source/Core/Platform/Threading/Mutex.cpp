/// Copyright (c) 2021 Shaye Garg.

#include "Core/Platform/Threading/Mutex.h"

#ifdef PLATFORM_WINDOWS

#	include <Windows.h>

namespace Ignis {

Mutex::Mutex() { InitializeSRWLock(reinterpret_cast<SRWLOCK*>(&m_PlatformHandle)); }

void Mutex::Lock() { AcquireSRWLockExclusive(reinterpret_cast<SRWLOCK*>(&m_PlatformHandle)); }

bool Mutex::TryLock() { return TryAcquireSRWLockExclusive(reinterpret_cast<SRWLOCK*>(&m_PlatformHandle)); }

void Mutex::Unlock() { ReleaseSRWLockExclusive(reinterpret_cast<SRWLOCK*>(&m_PlatformHandle)); }

}

#endif
