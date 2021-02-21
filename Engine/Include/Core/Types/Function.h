/// Copyright (c) 2021 Shaye Garg.
/// \file
/// Type-erased bound functions.

#pragma once
#include "Core/Memory/Memory.h"
#include "Core/Memory/RawAllocator.h"
#include "Core/Misc/Assert.h"

namespace Ignis {

namespace Private {

template<typename>
class Callable;

/// Base for all callable objects.
///
/// \tparam Ret Return value of the function.
/// \tparam Args Arguments for the function.
template<typename Ret, typename... Args>
class Callable<Ret(Args...)>
{
public:
	/// Destructor.
	virtual ~Callable() {}

	/// Invoke the callable object.
	///
	/// \param args Arguments to the callable.
	///
	/// \return Ret Return value.
	virtual Ret Invoke(Args... args) const = 0;

	/// Clone the callable object.
	///
	/// \param at Pointer to allocate the callable at.
	/// \param size Size of the memory region, in bytes.
	/// \param alloc Allocator to use to allocate memory for the callable if it does not fit in size bytes.
	///
	/// \return Pointer to the allocated callable. Free with Delete() if not equal to at.
	virtual Callable* Clone(void* at, u64 size, Allocator& alloc = GAlloc) const = 0;

	using Type = Ret(Args...);
};

template<typename, typename>
class FCallable;

/// Concrete callable type that holds anything with an operator().
///
/// \tparam T The type of the functor.
template<typename Ret, typename... Args, typename T>
class FCallable<Ret(Args...), T> : public Callable<Ret(Args...)>
{
public:
	FCallable(const T& callable) : m_Callable(callable) {}
	FCallable(T&& callable) : m_Callable(Move(callable)) {}

	Ret Invoke(Args... args) const override { return m_Callable(args...); }

	Callable<Ret(Args...)>* Clone(void* at, u64 size, Allocator& alloc = GAlloc) const override
	{
		if (size >= sizeof(*this))
		{
			return Construct<FCallable<Ret(Args...), T>>(at, m_Callable);
		}

		return New<FCallable<Ret(Args...), T>>(alloc, m_Callable);
	}

private:
	T m_Callable;
};

template<typename, typename>
class MCallable;

/// Concrete callable type that holds a member function and a pointer to the object to call the function on.
///
/// \tparam T Class to call the member function on.
template<typename Ret, typename... Args, typename T>
class MCallable<Ret(Args...), T> : public Callable<Ret(Args...)>
{
public:
	MCallable(Ret (T::*function)(Args...), T* object) : m_Object(object), m_Function(function) {}

	Ret Invoke(Args... args) const override
	{
		IASSERT(m_Object && m_Function, "MCallable is unbound!");
		return (m_Object->*m_Function)(args...);
	}

	Callable<Ret(Args...)>* Clone(void* at, u64 size, Allocator& alloc = GAlloc) const override
	{
		if (size >= sizeof(*this))
		{
			return Construct<MCallable<Ret(Args...), T>>(at, m_Function, m_Object);
		}

		return New<MCallable<Ret(Args...), T>>(alloc, m_Function, m_Object);
	}

private:
	T* m_Object;
	Ret (T::*m_Function)(Args...);
};

template<typename Ret, typename... Args>
auto DeduceCallable(Ret (*)(Args...)) -> Callable<Ret(Args...)>;

template<typename T, typename Ret, typename... Args>
auto DeduceCallableImpl(Ret (T::*)(Args...)) -> Callable<Ret(Args...)>;

template<typename T, typename Ret, typename... Args>
auto DeduceCallableImpl(Ret (T::*)(Args...) const) -> Callable<Ret(Args...)>;

template<typename C>
auto DeduceCallable(C) -> decltype(DeduceCallableImpl(&C::operator()));

}

template<typename>
class FunctionRef;

template<typename Ret, typename... Args>
class FunctionRef<Ret(Args...)>
{
public:
	FunctionRef() = default;
	FunctionRef(const Private::Callable<Ret(Args...)>& callable) : m_Callable(&callable) {}

	FunctionRef<Ret(Args...)>& operator=(const Private::Callable<Ret(Args...)>& callable)
	{
		m_Callable = &callable;
		return *this;
	}

	Ret operator()(Args... args) const
	{
		IASSERT(m_Callable, "FunctionRef is unbound!");
		return m_Callable->Invoke(args...);
	}

	const Private::Callable<Ret(Args...)>* GetCallable() { return m_Callable; }

private:
	const Private::Callable<Ret(Args...)>* m_Callable = nullptr;
};

template<typename>
class Function;

template<typename Ret, typename... Args>
class Function<Ret(Args...)>
{
public:
	template<typename T>
	Function(const T& callable, Allocator& alloc = GAlloc)
	{
		if (sizeof(T) > sizeof(m_Repr))
		{
			m_Repr.External = New<Private::FCallable<Ret(Args...), T>>(alloc, callable);
		}
		else
		{
			m_IsSmall = true;
			Construct<Private::FCallable<Ret(Args...), T>>(m_Repr.Internal, callable);
		}
	}

	template<typename T>
	Function(Ret (T::*function)(Args...), T* object, Allocator& alloc = GAlloc)
	{
		if (sizeof(T) > sizeof(m_Repr))
		{
			m_Repr.External = New<Private::MCallable<Ret(Args...), T>>(alloc, function, object);
		}
		else
		{
			m_IsSmall = true;
			Construct<Private::MCallable<Ret(Args...), T>>(m_Repr.Internal, function, object);
		}
	}

	Function(Function<Ret(Args...)>&& other)
	{
		m_Repr = other.m_Repr;
		m_IsSmall = other.m_IsSmall;
		other.m_IsSmall = true;
	}

	Function(const Function<Ret(Args...)>& other, Allocator& alloc = GAlloc)
	{
		auto callable = other.GetCallable()->Clone(m_Repr.Internal, sizeof(m_Repr), alloc);
		if (callable == reinterpret_cast<Private::Callable<Ret(Args...)>*>(m_Repr.Internal))
		{
			m_IsSmall = true;
		}
		else
		{
			m_Repr.External = callable;
		}
	}

	~Function()
	{
		if (!m_IsSmall)
		{
			Delete(m_Repr.External);
		}
	}

	Ret operator()(Args... args)
	{
		IASSERT(GetCallable(), "Function is unbound!");
		return GetCallable()->Invoke(args...);
	}

private:
	const Private::Callable<Ret(Args...)>* GetCallable() const
	{
		if (m_IsSmall)
		{
			return reinterpret_cast<const Private::Callable<Ret(Args...)>*>(m_Repr.Internal);
		}
		else
		{
			return m_Repr.External;
		}
	}

	union
	{
		u64 Internal[3];
		Private::Callable<Ret(Args...)>* External = nullptr;
	} m_Repr;
	bool m_IsSmall = false;
};

template<typename T>
auto Bind(const T& callable)
{
	using CallableType = typename decltype(Private::DeduceCallable(callable))::Type;
	return Private::FCallable<CallableType, T>(callable);
}

template<typename T, typename Ret, typename...Args>
auto Bind(Ret(T::*function)(Args...), T* obj)
{
	return Private::MCallable<Ret(Args...), T>(function, obj);
}

}
