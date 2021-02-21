/// Copyright (c) 2021 Shaye Garg.
/// \file
/// Descriptors for reflectable types.

#pragma once
#include "Core/Types/Array.h"
#include "Core/Types/Map.h"
#include "Core/Types/Pair.h"
#include "Core/Types/Pointer.h"
#include "Core/Types/String.h"

namespace Ignis {

struct TypeDescriptor;
template<typename>
struct PrimitiveDescriptor;
struct ClassDescriptor;
struct ArrayDescriptor;
struct PairDescriptor;
struct UniquePtrDescriptor;
struct SharedPtrDescriptor;

class DescriptorVisitor
{
public:
	virtual ~DescriptorVisitor() = default;

	/// Visit an i8.
	///
	/// \param desc Descriptor to visit.
	virtual void Visit(const PrimitiveDescriptor<i8>& desc) = 0;

	/// Visit an i16.
	///
	/// \param desc Descriptor to visit.
	virtual void Visit(const PrimitiveDescriptor<i16>& desc) = 0;

	/// Visit an i32.
	///
	/// \param desc Descriptor to visit.
	virtual void Visit(const PrimitiveDescriptor<i32>& desc) = 0;

	/// Visit an i64.
	///
	/// \param desc Descriptor to visit.
	virtual void Visit(const PrimitiveDescriptor<i64>& desc) = 0;

	/// Visit a u8.
	///
	/// \param desc Descriptor to visit.
	virtual void Visit(const PrimitiveDescriptor<u8>& desc) = 0;

	/// Visit a u16.
	///
	/// \param desc Descriptor to visit.
	virtual void Visit(const PrimitiveDescriptor<u16>& desc) = 0;

	/// Visit a u32.
	///
	/// \param desc Descriptor to visit.
	virtual void Visit(const PrimitiveDescriptor<u32>& desc) = 0;

	/// Visit a u64.
	///
	/// \param desc Descriptor to visit.
	virtual void Visit(const PrimitiveDescriptor<u64>& desc) = 0;

	/// Visit an f32.
	///
	/// \param desc Descriptor to visit.
	virtual void Visit(const PrimitiveDescriptor<f32>& desc) = 0;

	/// Visit an f64.
	///
	/// \param desc Descriptor to visit.
	virtual void Visit(const PrimitiveDescriptor<f64>& desc) = 0;

	/// Visit a Char.
	///
	/// \param desc Descriptor to visit.
	virtual void Visit(const PrimitiveDescriptor<Char>& desc) = 0;

	/// Visit a Byte.
	///
	/// \param desc Descriptor to visit.
	virtual void Visit(const PrimitiveDescriptor<Byte>& desc) = 0;

	/// Visit a String.
	///
	/// \param desc Descriptor to visit.
	virtual void Visit(const PrimitiveDescriptor<String>& desc) = 0;

	/// Visit an Array.
	///
	/// \param desc Descriptor to visit.
	virtual void Visit(const ArrayDescriptor& desc) = 0;

	/// Visit a Pair.
	///
	/// \param desc Descriptor to visit.
	virtual void Visit(const PairDescriptor& desc) = 0;

	/// Visit a UniquePtr.
	///
	/// \param desc Descriptor to visit.
	virtual void Visit(const UniquePtrDescriptor& desc) = 0;

	/// Visit a SharedPtr.
	///
	/// \param desc Descriptor to visit.
	virtual void Visit(const SharedPtrDescriptor& desc) = 0;

	/// Visit a class.
	///
	/// \param desc Descriptor to visit.
	virtual void Visit(const ClassDescriptor& desc) = 0;
};

/// Base descriptor for all reflectable types.
struct TypeDescriptor
{
	/// Constructor.
	///
	/// \param name Name of the type.
	/// \param size Size of the type.
	TypeDescriptor(StringRef name, u64 size) : Name(name), Size(size) {}

	/// Destructor.
	virtual ~TypeDescriptor() = default;

	/// Get the complete name of the type. Defaults to returning the name.
	/// Could be overridden to return the name of a templated type.
	/// For example, Array<String>'s Name would be Array, but GetName would return Array<String>.
	virtual String GetName() const { return Name; }

	/// Visit the TypeDescriptor.
	///
	/// \param visitor Visitor to visit it with.
	virtual void Visit(DescriptorVisitor& visitor) const = 0;

	/// Name of the type.
	StringRef Name;

	/// Size of the type.
	u64 Size;
};

template<>
struct PrimitiveDescriptor<i8> : TypeDescriptor
{
	PrimitiveDescriptor() : TypeDescriptor("i8", sizeof(i8)) {}

	void Visit(DescriptorVisitor& visitor) const override { visitor.Visit(*this); }
};

template<>
struct PrimitiveDescriptor<i16> : TypeDescriptor
{
	PrimitiveDescriptor() : TypeDescriptor("i16", sizeof(i16)) {}

	void Visit(DescriptorVisitor& visitor) const override { visitor.Visit(*this); }
};

template<>
struct PrimitiveDescriptor<i32> : TypeDescriptor
{
	PrimitiveDescriptor() : TypeDescriptor("i32", sizeof(i32)) {}

	void Visit(DescriptorVisitor& visitor) const override { visitor.Visit(*this); }
};

template<>
struct PrimitiveDescriptor<i64> : TypeDescriptor
{
	PrimitiveDescriptor() : TypeDescriptor("i64", sizeof(i64)) {}

	void Visit(DescriptorVisitor& visitor) const override { visitor.Visit(*this); }
};

template<>
struct PrimitiveDescriptor<u8> : TypeDescriptor
{
	PrimitiveDescriptor() : TypeDescriptor("u8", sizeof(i8)) {}

	void Visit(DescriptorVisitor& visitor) const override { visitor.Visit(*this); }
};

template<>
struct PrimitiveDescriptor<u16> : TypeDescriptor
{
	PrimitiveDescriptor() : TypeDescriptor("u16", sizeof(u16)) {}

	void Visit(DescriptorVisitor& visitor) const override { visitor.Visit(*this); }
};

template<>
struct PrimitiveDescriptor<u32> : TypeDescriptor
{
	PrimitiveDescriptor() : TypeDescriptor("u32", sizeof(u32)) {}

	void Visit(DescriptorVisitor& visitor) const override { visitor.Visit(*this); }
};

template<>
struct PrimitiveDescriptor<u64> : TypeDescriptor
{
	PrimitiveDescriptor() : TypeDescriptor("u64", sizeof(u64)) {}

	void Visit(DescriptorVisitor& visitor) const override { visitor.Visit(*this); }
};

template<>
struct PrimitiveDescriptor<f32> : TypeDescriptor
{
	PrimitiveDescriptor() : TypeDescriptor("f32", sizeof(f32)) {}

	void Visit(DescriptorVisitor& visitor) const override { visitor.Visit(*this); }
};

template<>
struct PrimitiveDescriptor<f64> : TypeDescriptor
{
	PrimitiveDescriptor() : TypeDescriptor("f64", sizeof(f64)) {}

	void Visit(DescriptorVisitor& visitor) const override { visitor.Visit(*this); }
};

template<>
struct PrimitiveDescriptor<Char> : TypeDescriptor
{
	PrimitiveDescriptor() : TypeDescriptor("Char", sizeof(Char)) {}

	void Visit(DescriptorVisitor& visitor) const override { visitor.Visit(*this); }
};

template<>
struct PrimitiveDescriptor<Byte> : TypeDescriptor
{
	PrimitiveDescriptor() : TypeDescriptor("Byte", sizeof(Byte)) {}

	void Visit(DescriptorVisitor& visitor) const override { visitor.Visit(*this); }
};

template<>
struct PrimitiveDescriptor<String> : TypeDescriptor
{
	PrimitiveDescriptor() : TypeDescriptor("String", sizeof(String)) {}

	void Visit(DescriptorVisitor& visitor) const override { visitor.Visit(*this); }
};

/// A member of a class or struct.
struct ClassMember
{
	/// Offset of the member from a pointer to the class.
	u64 Offset;

	/// TypeDescriptor of the member's type.
	const TypeDescriptor* Type;
};

/// Descriptor of a class or struct.
struct ClassDescriptor : TypeDescriptor
{
	/// Construct a class descriptor.
	///
	/// \param name Name of the class or struct.
	/// \param size Size of the class or struct.
	ClassDescriptor(StringRef name, u64 size) : TypeDescriptor(name, size) {}

	/// Public members of the class or struct.
	HashMap<StringRef, ClassMember> PublicMembers;

	/// Private and protected members of the class or struct.
	HashMap<StringRef, ClassMember> PrivateMembers;

	/// The class or struct derived from. Multiple inheritance is NOT supported , which is why only one parent pointer
	/// exists.
	ClassDescriptor* Parent = nullptr;

	void Visit(DescriptorVisitor& visitor) const override { visitor.Visit(*this); }
};

}
