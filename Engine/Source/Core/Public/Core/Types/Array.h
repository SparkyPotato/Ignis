/// Copyright (c) 2021 Shaye Garg.
/// \file
/// Arrays and ArrayRefs.

#pragma once
#include "Core/Memory/Memory.h"
#include "Core/Memory/RawAllocator.h"

namespace Ignis {

template<typename T>
class Array;

/// View into an Array, SafeArray, SmallArray, or StackArray.
/// You might know it as a Span.
///
/// \tparam T Type of the underlying Array.
template<typename T>
class ArrayRef
{
public:
	/// Construct an ArrayRef from a pointer and size.
	///
	/// \param data Pointer to first element in the sequence.
	/// \param size Number of elements in the sequence.
	ArrayRef(T* data, u64 size) : m_Data(data), m_Size(size) {}

	/// Construct an ArrayRef from an Array.
	///
	/// \param array Array to reference.
	ArrayRef(const Array<T>& array) : m_Data(array.Data()), m_Size(array.Size()) {}

	/// Index.
	///
	/// \param index The index of the element to access.
	///
	/// \return The element.
	T& operator[](u64 index) { return m_Data[index]; }

	/// Index.
	///
	/// \param index The index of the element to access.
	///
	/// \return The element.
	const T& operator[](u64 index) const { return m_Data[index]; }

	/// Get the size of the view.
	///
	/// \return The size.
	u64 Size() const { return m_Size; }

	/// Get the data the view points to.
	///
	/// \return Pointer to the first element of the view.
	const T* Data() const { return m_Data; }

	/// Iteration.
	///
	/// \return Iterator to the first element.
	const T* begin() const { return m_Data; }

	/// Iteration.
	///
	/// \return Beyond the end iterator.
	const T* end() const { return m_Data + m_Size; }

private:
	const T* m_Data = nullptr;
	u64 m_Size = 0;
};

template<typename T>
using Span = ArrayRef<T>;

template<typename T>
bool operator==(ArrayRef<T> first, ArrayRef<T> second)
{
	if (first.Size() != second.Size())
	{
		return false;
	}

	for (u64 i = 0; i < first.Size(); i++)
	{
		if (first[i] != second[i])
		{
			return false;
		}
	}

	return true;
}

template<typename T>
bool operator!=(ArrayRef<T> first, ArrayRef<T> second)
{
	return !(first == second);
}

/// Dynamically resizing array.
///
/// \tparam T Type to hold in the array.
template<typename T>
class Array
{
public:
	/// Construct an Array with an allocator.
	///
	/// \param alloc Allocator to use for memory allocation. Defaults to the Raw Allocator.
	Array(Allocator& alloc = GAlloc) : m_Alloc(&alloc)
	{
		m_Data = reinterpret_cast<T*>(m_Alloc->Allocate(sizeof(T) * 2));
		m_Size = 0;
		m_Capacity = 2;
	}

	/// Create an Array from an ArrayRef.
	///
	/// \param ref ArrayRef to create the string from.
	/// \param alloc Allocator to use for memory allocation. Defaults to the Raw Allocator.
	Array(ArrayRef<T> ref, Allocator& alloc = GAlloc) : m_Alloc(&alloc)
	{
		m_Data = reinterpret_cast<T*>(m_Alloc->Allocate(sizeof(T) * ref.Size()));
		m_Size = ref.Size();
		m_Capacity = ref.Size();

		// Copy elements
		for (u64 index = 0; auto& obj : ref)
		{
			Construct<T>(m_Data + index, obj);
			index++;
		}
	}

	/// Create an Array with a size.
	///
	/// \param size The size of the Array.
	/// \param alloc Allocator to use for memory allocation. Defaults to the Raw Allocator.
	///
	/// \warning This should only be used if you are writing directly to the Array's buffer with Data().
	Array(u64 size, Allocator& alloc = GAlloc) : m_Alloc(&alloc)
	{
		m_Data = reinterpret_cast<T*>(m_Alloc->Allocate(sizeof(T) * size));
		m_Size = size;
		m_Capacity = size;
	}

	/// Copy constructor.
	///
	/// \param other Array to copy.
	Array(const Array& other) : m_Alloc(other.m_Alloc)
	{
		m_Data = reinterpret_cast<T*>(m_Alloc->Allocate(sizeof(T) * other.m_Size));
		m_Size = other.m_Size;
		m_Capacity = other.m_Capacity;

		// Copy elements
		for (u64 index = 0; auto& obj : other)
		{
			Construct<T>(m_Data + index, obj);
			index++;
		}
	}

	/// Move constructor.
	///
	/// \param other Array to move.
	Array(Array&& other) : m_Alloc(other.m_Alloc)
	{
		m_Data = other.m_Data;
		m_Size = other.m_Size;
		m_Capacity = other.m_Capacity;
		other.m_Data = nullptr;
	}

	/// Destructor.
	~Array()
	{
		// Destroy objects
		for (auto& obj : *this)
		{
			obj.~T();
		}

		m_Alloc->Deallocate(m_Data);
	}

	/// Copy assignment.
	///
	/// \param other Array to copy.
	Array& operator=(const Array& other)
	{
		// Destroy objects
		for (auto& obj : *this)
		{
			obj->~T();
		}
		m_Size = 0;
		Realloc(other.m_Size);
		m_Size = other.m_Size;

		// Copy elements
		for (u64 index = 0; auto& obj : other)
		{
			Construct<T>(m_Data + index, obj);
			index++;
		}

		return *this;
	}

	/// Move assignment.
	///
	/// \param other Array to move.
	Array& operator=(Array&& other)
	{
		this->~Array<T>();

		m_Alloc = other.m_Alloc;
		m_Data = other.m_Data;
		m_Size = other.m_Size;
		m_Capacity = other.m_Capacity;
		other.m_Data = nullptr;

		return *this;
	}

	/// Index.
	///
	/// \param index The index of the element to access.
	///
	/// \return The element.
	T& operator[](u64 index) { return m_Data[index]; }

	/// Index.
	///
	/// \param index The index of the element to access.
	///
	/// \return The element.
	const T& operator[](u64 index) const { return m_Data[index]; }

	/// Get the first element of the Array.
	///
	/// \return Pointer to the first element of the Array.
	const T* Data() const { return m_Data; }

	/// Get the first element of the Array.
	///
	/// \return Pointer to the first element of the Array.
	T* Data() { return m_Data; }

	/// Get the number of elements in the Array.
	///
	/// \return The number of elements.
	u64 Size() const { return m_Size; }

	/// Reserve space in the Array.
	///
	/// \param size The number of elements to reserve space for.
	void Reserve(u64 size)
	{
		// Destroy objects
		for (auto& obj : *this)
		{
			obj.~T();
		}
		m_Size = 0;
	}

	/// Push a copy of an object into the end of the Array.
	///
	/// \param object Object to push.
	///
	/// \return Reference to the object in the Array.
	T& Push(const T& object)
	{
		Realloc(m_Size + 1);
		auto ptr = Construct<T>(m_Data + m_Size, object);
		m_Size++;
		return *ptr;
	}

	/// Construct an object in place at the end of the array.
	///
	/// \param args Arguments to pass to the constructor.
	///
	/// \return Constructed object.
	template<typename... Args>
	T& Emplace(Args&&... args)
	{
		Realloc(m_Size + 1);
		auto ptr = Construct<T>(m_Data + m_Size, static_cast<Args&&>(args)...);
		m_Size++;
		return *ptr;
	}

	/// Clear the Array, destroying all elements.
	void Clear();

	/// Iteration.
	///
	/// \return Pointer to the first element of the Array.
	T* begin() { return m_Data; }

	/// Iteration.
	///
	/// \return Pointer to the first element of the Array.
	const T* begin() const { return m_Data; }

	/// Iteration.
	///
	/// \return Pointer to the element after the last element in the Array.
	T* end() { return m_Data + m_Size; }

	/// Iteration.
	///
	/// \return Pointer to the element after the last element in the Array.
	const T* end() const { return m_Data + m_Size; }

private:
	/// Reallocate the vector if the capacity is not enough.
	/// Only updates m_Capacity.
	///
	/// \param size Number of elements required.
	void Realloc(u64 size)
	{
		if (size > m_Capacity)
		{
			u64 capacity = m_Capacity;
			while (capacity < size)
			{
				capacity *= 2;
			}

			u64 trySize = m_Alloc->GrowAllocation(m_Data, m_Capacity * sizeof(T), capacity * sizeof(T)) / sizeof(T);
			if (trySize < size)
			{
				auto data = reinterpret_cast<T*>(m_Alloc->Allocate(capacity * sizeof(T)));
				MemCopy(data, m_Data, m_Size * sizeof(T));
				for (u64 i = 0; i < m_Size; i++)
				{
					Construct<T>(data + i, Move(m_Data[i]));
				}
				m_Alloc->Deallocate(m_Data);
				m_Data = data;
				m_Capacity = capacity;
			}
			else
			{
				m_Capacity = trySize;
			}
		}
	}

	Allocator* m_Alloc = nullptr;
	T* m_Data = nullptr;
	u64 m_Size = 0;
	u64 m_Capacity = 0;
};

template<typename T>
bool operator==(Array<T> first, Array<T> second)
{
	if (first.Size() != second.Size())
	{
		return false;
	}

	for (u64 i = 0; i < first.Size(); i++)
	{
		if (first[i] != second[i])
		{
			return false;
		}
	}

	return true;
}

template<typename T>
bool operator!=(Array<T> first, Array<T> second)
{
	return !(first == second);
}

/// Array on the stack, prefer over Array if size is known at compile-time.
///
/// \tparam T Type to hold in the array.
/// \tparam S Number of elements in the array.
template<typename T, u64 S>
class StackArray
{
};

/// Array on the stack, up to S elements,
/// but switches over to dynamic allocation if the elements exceed S.
///
/// \tparam T Type to hold in the array.
/// \tparam S Number of elements to hold on the stack.
template<typename T, u64 S = 16>
class SmallArray
{
};

}
