/// Copyright (c) 2021 Shaye Garg.
/// \file
/// OS time.

#pragma once
#include "Core/Types/BaseTypes.h"

namespace Ignis {

class IGNIS_API Time
{
public:
	static Time Now();

	u16 Year;
	u8 Month;
	u8 Day;
	u8 WeekDay;
	u8 Hour;
	u8 Minute;
	u8 Second;
	u16 Millisecond;
};

}
