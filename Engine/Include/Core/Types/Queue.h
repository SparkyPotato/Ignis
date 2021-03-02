/// Copyright (c) 2021 Shaye Garg.
/// \file
/// Queues.

#pragma once
#include <atomic>

#include "Core/Memory/RawAllocator.h"

namespace Ignis {

/// A multi-producer, multi-consumer, single-ended, lockless queue.
template<typename T>
class MPMCQueue
{
public:
	MPMCQueue() = default;

	/// Construct an MPMCQueue.
	///
	/// \param size The number of elements to hold in the queue.
	/// \param alloc Allocator to use. Defaults to GAlloc.
	MPMCQueue(u64 size, Allocator& alloc = GAlloc) : m_Alloc(&alloc)
	{
		size--;
		size |= size >> 1;
		size |= size >> 2;
		size |= size >> 4;
		size |= size >> 8;
		size |= size >> 16;
		size |= size >> 32; // Round size to next multiple of 2 - 1.

		m_Mask = size;
		m_Capacity = size + 1;
		m_Slots = reinterpret_cast<Slot<T>*>(m_Alloc->Allocate(sizeof(Slot<T>) * (m_Capacity + 1)));
		for (u64 i = 0; i < m_Capacity; i++)
		{
			Construct<Slot<T>>(&m_Slots[i]);
		}
	}

	MPMCQueue(const MPMCQueue<T>& other) = delete;

	/// Destructor.
	~MPMCQueue()
	{
		if (!m_Slots)
		{
			return;
		}

		for (u64 i = 0; i < m_Capacity; i++)
		{
			m_Slots[i].~Slot<T>();
		}

		m_Alloc->Deallocate(m_Slots);
	}

	/// Move assignment.
	MPMCQueue<T>& operator=(MPMCQueue<T>&& other)
	{
		this->~MPMCQueue<T>();

		m_Slots = other.m_Slots;
		other.m_Slots = nullptr;
		m_Alloc = other.m_Alloc;
		m_Capacity = other.m_Capacity;
		m_Head = other.m_Head.load();
		m_Tail = other.m_Tail.load();
		m_Mask = other.m_Mask;

		return *this;
	}

	/// Emplace an object on the top of the queue. Spinlocks if the queue is full.
	///
	/// \param args Arguments to pass to the constructor.
	///
	/// \return Reference to the emplaced object.
	template<typename... Args>
	T& Emplace(Args&&... args)
	{
		u64 head = m_Head.fetch_add(1);
		auto& slot = m_Slots[Index(head)];
		while (Turn(head) * 2 != slot.Turn.load(std::memory_order::acquire)) {}

		auto ptr = Construct<T>(&slot.Storage, static_cast<Args&&>(args)...);
		slot.Turn.store(Turn(head) * 2 + 1, std::memory_order::release);

		return *ptr;
	}

	/// Try to emplace an object in the queue.
	///
	/// \param args Arguments to pass to the constructor.
	///
	/// \return If the emplace succeeded. If returns false, the queue is full.
	template<typename... Args>
	bool TryEmplace(Args&&... args)
	{
		u64 head = m_Head.load(std::memory_order::acquire);
		while (true)
		{
			auto& slot = m_Slots[Index(head)];
			if (Turn(head) * 2 == slot.Turn.load(std::memory_order::acquire))
			{
				if (m_Head.compare_exchange_strong(head, head + 1))
				{
					Construct<T>(&slot.Storage, static_cast<Args&&>(args)...);
					slot.Turn.store(Turn(head) * 2 + 1, std::memory_order::release);

					return true;
				}
			}
			else
			{
				u64 prev = head;
				head = m_Head.load(std::memory_order::acquire);
				if (head == prev) // Another thread didn't win, the queue is just full.
				{
					return false;
				}
			}
		}
	}

	/// Push a copy of an object on the end of the queue. Spinlocks if the queue is full.
	///
	/// \param obj Object to push.
	///
	/// \return Reference to the pushed object.
	T& Push(const T& obj) { return Emplace(obj); }

	/// Push a copy of an object on the end of the queue. Spinlocks if the queue is full.
	///
	/// \param obj Object to push.
	///
	/// \return Reference to the pushed object.
	T& Push(T&& obj) { return Emplace(std::move(obj)); }

	/// Try to push an object onto the queue.
	///
	/// \param obj Object to push.
	///
	/// \return If the push succeeded. If returns false, the queue was full.
	bool TryPush(const T& obj) { return TryEmplace(obj); }

	/// Try to push an object onto the queue.
	///
	/// \param obj Object to push.
	///
	/// \return If the push succeeded. If returns false, the queue was full.
	bool TryPush(T&& obj) { return TryEmplace(Move(obj)); }

	T Pop()
	{
		u64 tail = m_Tail.fetch_add(1);
		auto& slot = m_Slots[Index(tail)];
		while (Turn(tail) * 2 + 1 != slot.Turn.load(std::memory_order::acquire)) {}

		auto temp = std::move(slot.Storage);
		slot.Destroy();
		slot.Turn.store(Turn(tail) * 2 + 2, std::memory_order::release);

		return temp;
	}

	bool TryPop(T& obj)
	{
		u64 tail = m_Tail.load(std::memory_order::acquire);
		while (true)
		{
			auto& slot = m_Slots[Index(tail)];
			if (Turn(tail) * 2 + 1 == slot.Turn.load(std::memory_order::acquire))
			{
				if (m_Tail.compare_exchange_strong(tail, tail + 1))
				{
					obj = std::move(slot.Storage);
					slot.Destroy();
					slot.Turn.store(Turn(tail) * 2 + 2, std::memory_order::release);

					return true;
				}
			}
			else
			{
				u64 prev = tail;
				tail = m_Tail.load(std::memory_order::acquire);
				if (tail == prev)
				{
					return false;
				}
			}
		}
	}

private:
	template<typename S>
	class Slot
	{
	public:
		~Slot()
		{
			if (Turn & 1)
			{
				Destroy();
			}
		}

		void Destroy() { Storage.~S(); }

		alignas(64) std::atomic<u64> Turn = 0;
		S Storage;
	};

	constexpr u64 Index(u64 i) { return i & m_Mask; }
	constexpr u64 Turn(u64 i) { return i / m_Capacity; }

	Slot<T>* m_Slots = nullptr;
	u64 m_Capacity = 0;
	u64 m_Mask = 0;
	Allocator* m_Alloc = nullptr;

	alignas(64) std::atomic<u64> m_Head = 0;
	alignas(64) std::atomic<u64> m_Tail = 0;
};

}
