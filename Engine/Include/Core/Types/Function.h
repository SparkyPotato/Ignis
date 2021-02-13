/// Copyright (c) 2021 Shaye Garg.
/// \file
/// Type-erased bound functions.

#pragma once
#include "Core/Types/BaseTypes.h"
#include "Core/Misc/Assert.h"

namespace Ignis {

template<typename>
class FunctionRef;

/// Reference to a callable object. Does not support mutable lambdas.
template<typename Ret, typename... Args>
class FunctionRef<Ret(Args...)>
{
public:
	/// Type eraser for callable objects.
	class Callable
	{
	public:
		virtual ~Callable() = default;

		/// Invoke the callable object.
		///
		/// \param args Arguments to the invocation.
		/// 
		/// \return The return value of the call.
		virtual Ret Invoke(Args... args) const = 0;
	};

	/// Callable object for lambdas, functions pointers, and functors.
	/// Does not copy the object.
	template<typename T>
	class CallableObject : public Callable
	{
	public:
		/// Construct a CallableObject from any callable object.
		CallableObject(const T& callable) : m_Callable(callable) {}

		Ret Invoke(Args... args) const override { return m_Callable(static_cast<Args&&>(args)...); }

	private:
		const T& m_Callable;
	};

	/// Callable for member functions. 
	/// Does not copy the object.
	/// 
	/// \tparam C Class or struct of member function.
	template<typename C>
	class MemberFunction : public Callable
	{
	public:
		/// Construct a MemberFunction from an object and function.
		///
		/// \param obj Pointer to object to call the function on.
		/// \param func Function to call.
		MemberFunction(C* obj, Ret (C::*func)(Args&&...)) : m_Function(func), m_Obj(obj) {}

		Ret Invoke(Args... args) const override { return ((m_Obj)->*(m_Function))(static_cast<Args&&>(args)...); }

	private:
		Ret (C::*m_Function)(Args...);
		C* m_Obj;
	};

	/// Construct an unbound FunctionRef.
	FunctionRef() = default;

	/// Construct a FunctionRef to reference a callable.
	///
	/// \param callable Callable object to invoke.
	FunctionRef(const Callable& callable) { m_Callable = &callable; }

	template<typename C>
	FunctionRef& operator=(const Callable& callable)
	{
		m_Callable = &callable;
	}

	/// Invoke the enclosed callable reference.
	///
	/// \param args Arguments to invoke with.
	/// 
	/// \return Return value of the call.
	Ret operator()(Args... args) const
	{
		IASSERT(m_Callable, "FunctionRef is unbound");
		return m_Callable->Invoke(static_cast<Args&&>(args)...);
	}

	/// Check if the FunctionRef is valid.
	///
	/// \return If the FunctionRef is valid.
	operator bool() const { return m_Callable; }

private:
	const Callable* m_Callable = nullptr;
};

}
