/// Copyright (c) 2021 Shaye Garg.
/// \file
/// Smart pointers.

#pragma once
#include <atomic>

#include "Core/Memory/RawAllocator.h"

namespace Ignis {

/// Owning pointer with complete ownership.
///
/// \tparam T Type to point to.
template<typename T>
class Owner
{
public:
	Owner() = default;
	Owner(const Owner<T>& other) = delete;

	/// Implicit conversion between automatically convertible types.
	template<typename O>
	Owner(Owner<O>&& other)
	{
		m_Ptr = other.m_Ptr;
		m_Alloc = other.m_Alloc;
		other.m_Ptr = nullptr;
	}

	/// Destructor
	~Owner()
	{
		if (m_Ptr)
		{
			m_Ptr->~T();
			m_Alloc->Deallocate(m_Ptr);
		}
	}

	/// Dereference the pointer.
	///
	/// \return Reference to object.
	T& operator*() { return *m_Ptr; }

	/// Operator ->
	///
	/// \return Pointer to the object.
	T* operator->() { return m_Ptr; }

	/// Get the pointer held by the UniquePtr. Pointer retains ownership.
	///
	/// \return The pointer.
	T* Get() { return m_Ptr; }

private:
	template<typename O, typename... Args>
	friend Owner<O> MakeUnique(Args&&... args, Allocator& alloc);

	Owner(T* set, Allocator* alloc)
	{
		m_Ptr = set;
		m_Alloc = alloc;
	}

	T* m_Ptr = nullptr;
	Allocator* m_Alloc = nullptr;
};

/// Instantiate an object with single ownership.
/// 
/// \tparam T Type of object to instantiate.
/// \param args Arguments to pass to constructor.
/// \param alloc Allocator to use for allocating the object.
/// 
/// \return Owning pointer to the created object.
template<typename T, typename... Args>
Owner<T> MakeUnique(Args&&... args, Allocator& alloc = GAlloc)
{
	return Owner<T>(New<T>(static_cast<Args&&>(args)..., alloc), &alloc);
}

/// Reference counted pointer.
template<typename T>
class Handle
{
public:
	Handle() = default;

	/// Implicit conversion between automatically convertible types.
	template<typename O>
	Handle(const Handle<O>& other)
	{
		m_Ptr = other.m_Ptr;
		m_Ref = other.m_Ref
		(*m_Ref)++;
	}

	/// Implicit conversion between automatically convertible types.
	template<typename O>
	Handle(Handle<O>&& other)
	{
		m_Ptr = other.m_Ptr;
		m_Ref = other.m_Ref;
		other.m_Ptr = nullptr;
	}

	/// Destructor
	~Handle()
	{
		if (m_Ptr) 
		{
			(*m_Ref)--;
			if (!(*m_Ref)) // Destroy if refcount is 0.
			{
				Delete(m_Ptr);
				Delete(m_Ref);
			}
		}
	}

	/// Dereference the pointer.
	///
	/// \return Reference to object.
	T& operator*() { return *m_Ptr; }

	/// Operator ->
	///
	/// \return Pointer to the object.
	T* operator->() { return m_Ptr; }

private:
	template<typename O, typename... Args>
	friend Handle<O> MakeShared(Args&&... args, Allocator& alloc);

	Handle(T* ptr, std::atomic<u64>* ref) : m_Ptr(ptr), m_Ref(ref) {}

	T* m_Ptr;
	std::atomic<u64>* m_Ref;
};

/// Instantiate an object with shared ownership.
///
/// \tparam T Type of object to instantiate.
/// \param args Arguments to pass to constructor.
/// \param alloc Allocator to use for allocating the object.
///
/// \return Owning pointer to the created object.
template<typename O, typename... Args>
Handle<O> MakeShared(Args&&... args, Allocator& alloc)
{
	auto ptr = New<O>(static_cast<Args&&>(args)...);
	auto ref = New<std::atomic<u64>>(1);
	return Handle<O>(ptr, ref);
}

}
