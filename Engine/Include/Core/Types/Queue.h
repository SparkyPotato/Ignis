/// Copyright (c) 2021 Shaye Garg.
/// \file
/// Queues.

#pragma once
#include "Core/Threads/Atomic.h"

namespace Ignis {

/// A thread-safe fixed-size work stealing queue.
/// \tparam Object to hold in the queue. Should be an atomic type.
template<typename T>
class StealQueue
{
public:
	/// Construct a queue with a size.
	///
	/// \param size Maximum number of objects to hold.
	StealQueue(u64 size)
	{
		m_Size = size;
		m_Mask = size - 1;
		m_Data = reinterpret_cast<T*>(GAlloc.Allocate(size));
	}

	/// Push an object into the queue. Should only be called by the owning thread.
	///
	/// \param obj Object to push.
	void Push(const T& obj)
	{
		auto b = m_Bottom;
		m_Data[b & m_Mask] = obj;
		MemoryBlock(); // For you ARM folks out there
		m_Bottom = b + 1;
	}

	/// Pop an object from the queue. Should only be called by the owning thread.
	///
	/// \return The popped object. Default constructed if queue was empty.
	T Pop()
	{
		auto b = m_Bottom - 1;
		m_Bottom = b;
		auto t = m_Top;
		if (t <= b)
		{
			T obj = m_Data[b & m_Mask];
			if (t != b)
			{
				return obj;
			}

			if (m_Top.CompareExchange(t + 1, t) != t)
			{
				obj = T{};
			}

			m_Bottom = t + 1;
			return obj;
		}
		m_Bottom = t;
		return T{};
	}

	/// Steal an object from the queue. Can be called by any thread.
	///
	/// \return The stolen object. Default constructed if queue was empty.
	T Steal()
	{
		auto t = m_Top;
		MemoryBlock();
		auto b = m_Bottom;
		if (t < b)
		{
			T obj = m_Data[t & m_Mask];
			if (m_Top.CompareExchange(t + 1, t) != t)
			{
				return T{};
			}
			return obj;
		}
		return T{};
	}

private:
	T* m_Data;
	u64 m_Size;
	u64 m_Top = 0;
	u64 m_Bottom = 0;
	u64 m_Mask;
};

}
