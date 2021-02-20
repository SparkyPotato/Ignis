/// Copyright (c) 2021 Shaye Garg.
/// \file
/// Reflectors and reflection type resolvers.

#pragma once
#include "Core/Types/Traits.h"

#include "Core/Reflection/Descriptor.h"

namespace Ignis {

template<typename T>
concept CustomType = requires()
{
	{
		&T::ReflectionDescriptor
	}
	->std::convertible_to<TypeDescriptor*>;
};

template<typename T>
concept PrimitiveType = Traits::IsComplete<PrimitiveDescriptor<T>>::value;

template<PrimitiveType>
class PrimitiveResolver;

class DefaultResolver
{
public:
	template<PrimitiveType T>
	static const TypeDescriptor* Get()
	{
		return PrimitiveResolver<T>::Get();
	}

	template<CustomType T>
	static const TypeDescriptor* Get()
	{
		return &T::ReflectionDescriptor;
	}

	template<typename T>
	static const TypeDescriptor* Get()
	{
		static_assert(false, "T is not reflectable!");
	}
};

template<typename T>
class Resolver
{
public:
	static const TypeDescriptor* Get() { return DefaultResolver::Get<T>(); }
};

template<PrimitiveType T>
class PrimitiveResolver
{
public:
	static const TypeDescriptor* Get() { return &m_Desc; }

private:
	const static inline PrimitiveDescriptor<T> m_Desc;
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
class Resolver<Owner<T>>
{
public:
	static const TypeDescriptor* Get() { return &m_Desc; }

private:
	const static inline OwnerDescriptor m_Desc = OwnerDescriptor((T*) nullptr);
};

template<typename T>
class Resolver<Handle<T>>
{
public:
	static const TypeDescriptor* Get() { return &m_Desc; }

private:
	const static inline HandleDescriptor m_Desc = HandleDescriptor((T*) nullptr);
};

}
