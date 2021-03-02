/// Copyright (c) 2021 Shaye Garg.

#include "Core/Types/Time.h"

#include "Core/Platform/Platform.h"

#ifdef PLATFORM_WINDOWS
#	include <Windows.h>
#endif

namespace Ignis {

#ifdef PLATFORM_WINDOWS

Time Time::Now()
{
	SYSTEMTIME time;
	GetLocalTime(&time);
	return Time{ .Year = time.wYear,
		.Month = u8(time.wMonth),
		.Day = u8(time.wDay),
		.WeekDay = u8(time.wDayOfWeek),
		.Hour = u8(time.wHour),
		.Minute = u8(time.wMinute),
		.Millisecond = time.wMilliseconds };
}

#endif

}
