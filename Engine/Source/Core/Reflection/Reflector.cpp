/// Copyright (c) 2021 Shaye Garg.

#include "Core/Reflection/Reflector.h"

namespace Ignis {

Reflector::Reflector(void* object, ClassDescriptor* descriptor) : m_Object(object), m_Descriptor(descriptor) {}

namespace Private {

// Do nothing
void ReflectorVisitor::Visit(const SharedPtrDescriptor& desc) {}
void ReflectorVisitor::Visit(const UniquePtrDescriptor& desc) {}
void ReflectorVisitor::Visit(const PairDescriptor& desc) {}
void ReflectorVisitor::Visit(const ArrayDescriptor& desc) {}
void ReflectorVisitor::Visit(const PrimitiveDescriptor<String>& desc) {}
void ReflectorVisitor::Visit(const PrimitiveDescriptor<Byte>& desc) {}
void ReflectorVisitor::Visit(const PrimitiveDescriptor<Char>& desc) {}
void ReflectorVisitor::Visit(const PrimitiveDescriptor<f64>& desc) {}
void ReflectorVisitor::Visit(const PrimitiveDescriptor<f32>& desc) {}
void ReflectorVisitor::Visit(const PrimitiveDescriptor<u64>& desc) {}
void ReflectorVisitor::Visit(const PrimitiveDescriptor<u32>& desc) {}
void ReflectorVisitor::Visit(const PrimitiveDescriptor<u16>& desc) {}
void ReflectorVisitor::Visit(const PrimitiveDescriptor<u8>& desc) {}
void ReflectorVisitor::Visit(const PrimitiveDescriptor<i64>& desc) {}
void ReflectorVisitor::Visit(const PrimitiveDescriptor<i32>& desc) {}
void ReflectorVisitor::Visit(const PrimitiveDescriptor<i16>& desc) {}
void ReflectorVisitor::Visit(const PrimitiveDescriptor<i8>& desc) {}

void ReflectorVisitor::Visit(const ClassDescriptor& desc) { IsClass = true; }

}

}
