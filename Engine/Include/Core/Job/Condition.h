/// Copyright (c) 2021 Shaye Garg.
/// \file
/// A wake condition for a sleeping job.

#pragma once

namespace Ignis {

class IGNIS_API WaitCondition
{
public:
	virtual ~WaitCondition() = default;

	/// Check if the condition has been satisfied.
	/// 
	/// \return If the condition has been satisfied and the job can wake up again.
	virtual operator bool() const = 0;

	/// Put the calling THREAD to sleep till the condition is satisfied.
	virtual void SleepOn() const = 0;
};

}
