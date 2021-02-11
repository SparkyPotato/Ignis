/// Copyright (c) 2021 Shaye Garg.
/// \file
/// Types for holding any other types.

#pragma once

namespace Ignis {

/// Reference to any type.
class AnyRef
{
public:
	AnyRef() = default;

	/// Construct an AnyRef.
	///
	/// \param value Value to reference. Must be valid as long as the AnyRef will be used.
	template<typename T>
	AnyRef(const T& value) : m_Data(&value), m_Manager(&Manager<T>::Manage)
	{
	}

	template<typename T>
	AnyRef& operator=(const T& value)
	{
		m_Data = &value;
		m_Manager = &Manager<T>::Manage;
	}

	/// Get the contained reference.
	///
	/// \return Pointer to the object, nullptr if the requested type does not match the contained type.
	template<typename T>
	const T* Get() const
	{
		if (m_Manager == &Manager<T>::Manage)
		{
			return reinterpret_cast<const T*>(m_Data);
		}

		return nullptr;
	}

private:
	template<typename T>
	class Manager
	{
	public:
		static void Manage(AnyRef* any) {}
	};

	const void* m_Data = nullptr;
	void (*m_Manager)(AnyRef*) = nullptr;
};

}
