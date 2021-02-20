/// Copyright (c) 2021 Shaye Garg.

#include "Core/Platform/Threading/Thread.h"

#include "Core/Platform/Platform.h"
#include "Core/Platform/Internals.h"

#ifdef PLATFORM_WINDOWS

#	include <Windows.h>

namespace Ignis {

struct ThreadData
{
	Function<void()> Func;
	HANDLE StartSemaphore;
};

static DWORD ThreadProc(void* arg)
{
	// Take a copy so that the constructor can unwind its stack
	auto data = *reinterpret_cast<ThreadData*>(arg);
	ReleaseSemaphore(data.StartSemaphore, 1, nullptr);

	data.Func();

	return 0;
}

Thread::Thread(Function<void()> threadFunction)
{
	ThreadData data{ threadFunction, CreateSemaphoreW(nullptr, 0, 1, nullptr) };

	m_PlatformHandle = CreateThread(nullptr, 0, &ThreadProc, &data, 0, nullptr);
	if (!m_PlatformHandle)
	{
		return;
	}
	m_ID = GetThreadId(m_PlatformHandle);

	// Wait until ThreadProc has gotten a copy of the required data
	WaitForSingleObject(data.StartSemaphore, INFINITE);
	CloseHandle(data.StartSemaphore);
}

Thread::~Thread()
{
	DWORD exit;
	GetExitCodeThread(m_PlatformHandle, &exit);
	if (exit == STILL_ACTIVE) 
	{
		TerminateThread(m_PlatformHandle, 0);
		CloseHandle(m_PlatformHandle);
	}
}

void Thread::Join() { WaitForSingleObject(m_PlatformHandle, INFINITE); }

void Thread::Detach()
{
	CloseHandle(m_PlatformHandle);
	m_PlatformHandle = nullptr;
	m_ID = 0;
}

void Thread::SetName(StringRef name)
{
	SetThreadDescription(
		m_PlatformHandle, reinterpret_cast<const wchar_t*>(PlatformInternals::ConvToUTF16(name).CString()));
}

u16 Thread::GetMaxThreads()
{
	SYSTEM_INFO info;
	GetSystemInfo(&info);
	return u16(info.dwNumberOfProcessors);
}

}

#else

#	include<atomic>
#	include <pthread.h>
#	include <sys/sysinfo.h>

namespace Ignis {

struct ThreadData
{
	Function<void()> Func;
	std::atomic<u64> StartSemaphore;
};

static void* ThreadFunction(void* arg)
{
	auto data = reinterpret_cast<ThreadData*>(arg);
	auto func = data->Func;
	data->StartSemaphore--;

	func();

	return nullptr;
}

Thread::Thread(Function<void()> threadFunction)
{
	ThreadData data{ threadFunction, 1 };

	pthread_create(reinterpret_cast<pthread_t*>(&m_PlatformHandle), nullptr, ThreadFunction, &data);
	if (!m_PlatformHandle)
	{
		return;
	}

	// Wait until ThreadProc has gotten a copy of the required data
	while (data.StartSemaphore > 0) {}
}

Thread::~Thread()
{
	pthread_cancel(*reinterpret_cast<pthread_t*>(&m_PlatformHandle));
	pthread_detach(*reinterpret_cast<pthread_t*>(&m_PlatformHandle));
}

void Thread::Join() { pthread_join(*reinterpret_cast<pthread_t*>(&m_PlatformHandle), nullptr); }

void Thread::Detach()
{
	pthread_detach(*reinterpret_cast<pthread_t*>(&m_PlatformHandle));
	m_PlatformHandle = nullptr;
}

void Thread::SetName(StringRef name) {}

u16 Thread::GetMaxThreads()
{
#	ifdef PLATFORM_LINUX
	return u16(get_nprocs()); // Since we only support GCC and Clang on Linux.
#	else // On macOS
	int count;
	size_t size = sizeof(count);
	return u16(sysctlbyname("hw.ncpu", &count, &size, nullptr, 0) ? 0 : count);
#	endif
}

}

#endif
