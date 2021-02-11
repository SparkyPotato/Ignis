/// Copyright (c) 2021 Shaye Garg.
/// \file
/// Smart pointers.

#pragma once
#include "Core/Memory/RawAllocator.h"

namespace Ignis {

/// Owning pointer with complete ownership.
///
/// \tparam T Type to point to.
template<typename T>
class UniquePtr
{
public:
	UniquePtr() = default;
	UniquePtr(const UniquePtr<T>& other) = delete;

	/// Implicit conversion between automatically convertible types.
	template<typename O>
	UniquePtr(UniquePtr<O>&& other)
	{
		m_Ptr = other.m_Ptr;
		m_Alloc = other.m_Alloc;
		other.m_Ptr = nullptr;
	}

	/// Destructor
	~UniquePtr()
	{
		if (m_Ptr)
		{
			m_Ptr->~T();
			m_Alloc->Deallocate(m_Ptr);
		}
	}

	T& operator*() { return *m_Ptr; }

	T* operator->() { return m_Ptr; }
	
	/// Get the pointer held by the UniquePtr. Pointer retains ownership.
	/// 
	/// \return The pointer.
	T* Get() { return m_Ptr; }

private:
	template<typename O, typename... Args>
	friend UniquePtr<O> MakeUnique(Args&&... args, Allocator& alloc);

	UniquePtr(T* set, Allocator* alloc)
	{
		m_Ptr = set;
		m_Alloc = alloc;
	}

	T* m_Ptr = nullptr;
	Allocator* m_Alloc = nullptr;
};

template<typename T, typename... Args>
UniquePtr<T> MakeUnique(Args&&... args, Allocator& alloc = GAlloc)
{
	return UniquePtr<T>(Construct<T>(alloc.Allocate(sizeof(T)), static_cast<Args&&>(args)...), &alloc);
}

}
