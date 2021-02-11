/// Copyright (c) 2021 Shaye Garg.

#include "Core/Threads/Thread.h"

#include "Core/Platform/Internals.h"
#include "Core/Platform/Platform.h"

#ifdef PLATFORM_WINDOWS

#	include <Windows.h>

namespace Ignis {

struct ThreadData
{
	FunctionRef<void()> Function;
	HANDLE StartSemaphore;
};

static DWORD ThreadProc(void* arg)
{
	// Take a copy so that the constructor can unwind its stack
	auto data = *reinterpret_cast<ThreadData*>(arg);
	ReleaseSemaphore(data.StartSemaphore, 1, nullptr);

	data.Function();

	return 0;
}

Thread::Thread(FunctionRef<void()> threadFunction)
{
	ThreadData data{ threadFunction, CreateSemaphoreW(nullptr, 0, 1, nullptr) };

	m_PlatformHandle = CreateThread(nullptr, 0, &ThreadProc, &data, 0, nullptr);
	m_ID = GetThreadId(m_PlatformHandle);

	// Wait until ThreadProc has gotten a copy of the required data
	WaitForSingleObject(data.StartSemaphore, INFINITE);
	CloseHandle(data.StartSemaphore);
}

Thread::~Thread()
{
	TerminateThread(m_PlatformHandle, 0);
	CloseHandle(m_PlatformHandle);
}

void Thread::Join() { WaitForSingleObject(m_PlatformHandle, INFINITE); }

void Thread::Detach()
{
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

#endif
