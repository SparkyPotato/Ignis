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
		other.m_Ptr = nullptr;
	}

	/// Destructor
	~UniquePtr() { Delete(m_Ptr); }

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
	friend UniquePtr<O> MakeUnique(Args&&... args, Allocator& alloc);

	UniquePtr(T* set) { m_Ptr = set; }

	T* m_Ptr = nullptr;
};

/// Instantiate an object with single ownership.
///
/// \tparam T Type of object to instantiate.
/// \param args Arguments to pass to constructor.
/// \param alloc Allocator to use for allocating the object.
///
/// \return Owning pointer to the created object.
template<typename T, typename... Args>
UniquePtr<T> MakeUnique(Args&&... args, Allocator& alloc = GAlloc)
{
	return UniquePtr<T>(New<T>(static_cast<Args&&>(args)..., alloc));
}

/// Reference counted pointer.
template<typename T>
class SharedPtr
{
public:
	SharedPtr() = default;

	/// Implicit conversion between automatically convertible types.
	template<typename O>
	SharedPtr(const SharedPtr<O>& other)
	{
		m_Ptr = other.m_Ptr;
		m_Ref = other.m_Ref(*m_Ref)++;
	}

	/// Implicit conversion between automatically convertible types.
	template<typename O>
	SharedPtr(SharedPtr<O>&& other)
	{
		m_Ptr = other.m_Ptr;
		m_Ref = other.m_Ref;
		other.m_Ptr = nullptr;
	}

	/// Destructor
	~SharedPtr()
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
	friend SharedPtr<O> MakeShared(Args&&... args, Allocator& alloc);

	SharedPtr(T* ptr, std::atomic<u64>* ref) : m_Ptr(ptr), m_Ref(ref) {}

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
SharedPtr<O> MakeShared(Args&&... args, Allocator& alloc)
{
	auto ptr = New<O>(alloc, static_cast<Args&&>(args)...);
	auto ref = New<std::atomic<u64>>(alloc, 1);
	return SharedPtr<O>(ptr, ref);
}

template<typename>
class Array;

/// Pointer to an element in an Array.
///
/// \tparam Type held in the Array.
template<typename T>
class ArrayPtr
{
public:
	ArrayPtr() = default;

	/// Constructor.
	///
	/// \param array Array to point to.
	/// \param index Index of the object in the array.
	ArrayPtr(Array<T>* array, u64 index) : m_Array(array), m_Index(index) {}

	/// Contructor.
	///
	/// \param array Array to point to.
	/// \param pointer Pointer to the object to point to.
	ArrayPtr(Array<T>* array, T* pointer) : m_Array(array), m_Index(pointer - array->Data()) {}

	T& operator*()
	{
		IASSERT(m_Array, "ArrayPtr is nullptr!");
		return m_Array[m_Index];
	}

	T* operator->()
	{
		IASSERT(m_Array, "ArrayPtr is nullptr!");
		return &m_Array[m_Index];
	}

private:
	Array<T>* m_Array = nullptr;
	u64 m_Index;
};

}
