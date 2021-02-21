/// Copyright (c) 2021 Shaye Garg.
/// \file
/// Reflector for getting access to a reflectable object.

#pragma once
#include "Core/Reflection/Resolver.h"

namespace Ignis {

namespace Private {

class IGNIS_API ReflectorVisitor : public DescriptorVisitor
{
public:
	void Visit(const PrimitiveDescriptor<i8>& desc) override;
	void Visit(const PrimitiveDescriptor<i16>& desc) override;
	void Visit(const PrimitiveDescriptor<i32>& desc) override;
	void Visit(const PrimitiveDescriptor<i64>& desc) override;
	void Visit(const PrimitiveDescriptor<u8>& desc) override;
	void Visit(const PrimitiveDescriptor<u16>& desc) override;
	void Visit(const PrimitiveDescriptor<u32>& desc) override;
	void Visit(const PrimitiveDescriptor<u64>& desc) override;
	void Visit(const PrimitiveDescriptor<f32>& desc) override;
	void Visit(const PrimitiveDescriptor<f64>& desc) override;
	void Visit(const PrimitiveDescriptor<Char>& desc) override;
	void Visit(const PrimitiveDescriptor<Byte>& desc) override;
	void Visit(const PrimitiveDescriptor<String>& desc) override;
	void Visit(const ArrayDescriptor& desc) override;
	void Visit(const PairDescriptor& desc) override;
	void Visit(const UniquePtrDescriptor& desc) override;
	void Visit(const SharedPtrDescriptor& desc) override;
	void Visit(const ClassDescriptor& desc) override;

	bool IsClass = false;
};

}

/// Reflector for getting references to properties of reflectable types.
class Reflector
{
public:
	/// Constructor.
	///
	/// \param object Pointer to the object to reflect.
	/// \param descriptor TypeDescriptor of the object.
	Reflector(void* object, ClassDescriptor* descriptor);

	/// Constructor.
	///
	/// \param object Object to reflect.
	template<Reflectable T>
	Reflector(T& object) : m_Object(&object)
	{
		Private::ReflectorVisitor visitor;
		Resolver<T>::Get()->Visit(visitor);
		IASSERT(visitor.IsClass, "Reflector can only reflect classes or structs!");

		m_Descriptor = reinterpret_cast<const ClassDescriptor*>(Resolver<T>::Get());
	}

	/// Get a property from the type descriptor.
	///
	/// \param name Name of the property.
	/// 
	/// \return Pointer to the property. Is nullptr if it doesn't exist.
	template<typename T>
	T* Get(StringRef name)
	{
		auto member = m_Descriptor->PublicMembers.Get(name);
		if (member)
		{
			reinterpret_cast<T*>(reinterpret_cast<u8*>(m_Object) + member->Offset);
		}

		return nullptr;
	}

private:
	void* m_Object;
	const ClassDescriptor* m_Descriptor;
};

}
