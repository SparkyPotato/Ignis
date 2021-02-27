/// Copyright (c) 2021 Shaye Garg.
/// \file
/// Reflectors and reflection type resolvers.

#pragma once
#include "Core/Types/Traits.h"

#include "Core/Reflection/Descriptor.h"

namespace Ignis {

template<typename T>
concept CustomReflectable = requires()
{
	{
		&T::ReflectionDescriptor
	}
	->std::convertible_to<TypeDescriptor*>;
};

template<typename T>
concept PrimitiveReflectable = Traits::IsComplete<PrimitiveDescriptor<T>>::value;

template<typename T>
concept Reflectable = CustomReflectable<T> || PrimitiveReflectable<T>;

template<PrimitiveReflectable>
class PrimitiveResolver;

class DefaultResolver
{
public:
	template<PrimitiveReflectable T>
	static const TypeDescriptor* Get()
	{
		return PrimitiveResolver<T>::Get();
	}

	template<CustomReflectable T>
	static const TypeDescriptor* Get()
	{
		return &T::ReflectionDescriptor;
	}
};

template<typename T>
class Resolver
{
public:
	static const TypeDescriptor* Get() { return DefaultResolver::Get<T>(); }
};

template<PrimitiveReflectable T>
class PrimitiveResolver
{
public:
	static const TypeDescriptor* Get() { return &m_Desc; }

private:
	const static inline PrimitiveDescriptor<T> m_Desc;
};

struct ArrayDescriptor : TypeDescriptor
{
	template<typename T>
	ArrayDescriptor(T* holdingTypeForImplicitTemplates)
		: TypeDescriptor("Array<>", sizeof(Array<T>)), Holding(Resolver<T>::Get())
	{
	}

	String GetName() const override { return "Array<" + Holding->GetName() + ">"; }

	void Visit(DescriptorVisitor& visitor) const override { visitor.Visit(*this); }

	/// The descriptor of the type the Array is holding.
	const TypeDescriptor* Holding;
};

struct PairDescriptor : TypeDescriptor
{
	template<typename T, typename U>
	PairDescriptor(T* implicit1, U* implicit2)
		: TypeDescriptor("Pair<>", sizeof(Pair<T, U>)), First(Resolver<T>::Get()), Second(Resolver<U>::Get())
	{
	}

	String GetName() const override { return "Pair<" + First->GetName() + ", " + Second->GetName() + ">"; }

	void Visit(DescriptorVisitor& visitor) const override { visitor.Visit(*this); }

	/// First type in the pair.
	const TypeDescriptor* First;

	/// Second type in the pair.
	const TypeDescriptor* Second;
};

struct UniquePtrDescriptor : TypeDescriptor
{
	template<typename T>
	UniquePtrDescriptor(T* implicit) : TypeDescriptor("UniquePtr<>", sizeof(UniquePtr<T>)), Holding(Resolver<T>::Get())
	{
	}

	String GetName() const override { return "UniquePtr<" + Holding->GetName() + ">"; }

	void Visit(DescriptorVisitor& visitor) const override { visitor.Visit(*this); }

	/// The descriptor of the type the UniquePtr is holding.
	const TypeDescriptor* Holding;
};

struct SharedPtrDescriptor : TypeDescriptor
{
	template<typename T>
	SharedPtrDescriptor(T* implicit) : TypeDescriptor("SharedPtr<>", sizeof(SharedPtr<T>)), Holding(Resolver<T>::Get())
	{
	}

	String GetName() const override { return "SharedPtr<" + Holding->GetName() + ">"; }

	void Visit(DescriptorVisitor& visitor) const override { visitor.Visit(*this); }

	/// The descriptor of the type the SharedPtr is holding.
	const TypeDescriptor* Holding;
};

template<typename T>
class Resolver<Array<T>>
{
public:
	static const TypeDescriptor* Get() { return &m_Desc; }

private:
	const static inline ArrayDescriptor m_Desc = ArrayDescriptor((T*) nullptr);
};

template<typename T, typename U>
class Resolver<Pair<T, U>>
{
public:
	static const TypeDescriptor* Get() { return &m_Desc; }

private:
	const static inline PairDescriptor m_Desc = PairDescriptor((T*) nullptr, (U*) nullptr);
};

template<typename T>
class Resolver<UniquePtr<T>>
{
public:
	static const TypeDescriptor* Get() { return &m_Desc; }

private:
	const static inline UniquePtrDescriptor m_Desc = UniquePtrDescriptor((T*) nullptr);
};

template<typename T>
class Resolver<SharedPtr<T>>
{
public:
	static const TypeDescriptor* Get() { return &m_Desc; }

private:
	const static inline SharedPtrDescriptor m_Desc = SharedPtrDescriptor((T*) nullptr);
};

}
