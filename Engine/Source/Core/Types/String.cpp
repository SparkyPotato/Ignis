/// Copyright (c) 2021 Shaye Garg.

#include "Core/Types/String.h"

#include "Core/Memory/Memory.h"
#include "Core/Misc/Assert.h"

namespace Ignis {

u64 StrLen(const char* ptr)
{
	u64 len = 0;
	for (; ptr[len] != '\0'; len++) {}
	return len;
}

StringIterator::StringIterator(const Byte* b) : m_Byte(b) {}

Char StringIterator::operator*() const
{
	if ((*m_Byte & 0b10000000) == 0) // 0xxxxxxx
	{
		return *m_Byte;
	}
	else if ((*m_Byte & 0b11100000) == 0b11000000) // 110xxxxx
	{
		return ((*m_Byte & 0b00011111) << 6) + (*(m_Byte + 1) & 0b00111111);
	}
	else if ((*m_Byte & 0b11110000) == 0b11100000) // 1110xxxx
	{
		return ((*m_Byte & 0b00001111) << 12) + ((*(m_Byte + 1) & 0b00111111) << 6) + (*(m_Byte + 2) & 0b00111111);
	}
	else if ((*m_Byte & 0b11111000) == 0b11110000) // 11110xxx
	{
		return ((*m_Byte & 0b00000111) << 18) + ((*(m_Byte + 1) & 0b00111111) << 12)
			   + ((*(m_Byte + 2) & 0b00111111) << 6) + (*(m_Byte + 3) & 0b00111111);
	}
	else
	{
		IASSERT(false, "Invalid byte");
		return 0xfffd;
	}
}

StringIterator& StringIterator::operator++()
{
	if ((*m_Byte & 0b11000000) == 0b10000000) // 10xxxxxx
	{
		IASSERT(false, "Inside codepoints");
	}
	else if ((*m_Byte & 0b10000000) == 0) // 0xxxxxxx
	{
		m_Byte++;
	}
	else if ((*m_Byte & 0b11100000) == 0b11000000) // 110xxxxx
	{
		m_Byte += 2;
	}
	else if ((*m_Byte & 0b11110000) == 0b11100000) // 1110xxxx
	{
		m_Byte += 3;
	}
	else if ((*m_Byte & 0b11111000) == 0b11110000) // 11110xxx
	{
		m_Byte += 4;
	}
	else
	{
		IASSERT(false, "Invalid byte");
	}

	return *this;
}

StringIterator StringIterator::operator++(int)
{
	StringIterator temp = *this;
	++(*this);
	return temp;
}

StringIterator StringIterator::operator+(u64 offset) const
{
	auto it = *this;
	for (; offset > 0; offset--)
	{
		it++;
	}
	return it;
}

StringIterator& StringIterator::operator+=(u64 offset)
{
	for (; offset > 0; offset--)
	{
		(*this)++;
	}
	return *this;
}

StringRef::StringRef(const char* ptr) : m_Data(reinterpret_cast<const Byte*>(ptr)) { m_Size = StrLen(ptr); }

StringRef::StringRef(const char* ptr, u64 size) : m_Data(reinterpret_cast<const Byte*>(ptr)), m_Size(size) {}

StringRef::StringRef(const String& string) : m_Data(string.CString()), m_Size(string.Size()) {}

String StringRef::operator+(StringRef ref) const { return String(*this) + ref; }

const Byte* StringRef::Data() const { return m_Data; }
u64 StringRef::Size() const { return m_Size; }

StringRef::Iterator StringRef::begin() const { return Iterator(m_Data); }

StringRef::Iterator StringRef::end() const { return Iterator(m_Data + m_Size); }

String::Repr::Repr() { MemSet(this, 0, sizeof(this)); }

String::String(Allocator& alloc) : m_Alloc(&alloc)
{
	SetSmall(true);
	m_Repr.Small.Data[m_SmallSize] = m_SmallSize;
}

String::String(const char* cptr, Allocator& alloc) : m_Alloc(&alloc)
{
	u64 size = StrLen(cptr);
	const Byte* ptr = reinterpret_cast<const Byte*>(cptr);

	if (size > m_SmallSize) // Big string
	{
		SetSmall(false);
		m_Repr.Big.Data = reinterpret_cast<Byte*>(GetAlloc()->Allocate(size + 1));
		MemCopy(m_Repr.Big.Data, ptr, size);
		m_Repr.Big.Data[size] = '\0'; // StringViews are not null-terminated
		m_Repr.Big.Size = size;
		m_Repr.Big.Capacity = size;
	}
	else
	{
		SetSmall(true);
		MemCopy(m_Repr.Small.Data, ptr, size + 1);
		m_Repr.Small.Data[size] = '\0';
		m_Repr.Small.Data[m_SmallSize] = m_SmallSize - Byte(size);
	}
}

String::String(StringRef view, Allocator& alloc) : m_Alloc(&alloc)
{
	u64 size = view.Size();
	const Byte* ptr = view.Data();

	if (size > m_SmallSize) // Big string
	{
		SetSmall(false);
		m_Repr.Big.Data = reinterpret_cast<Byte*>(GetAlloc()->Allocate(size + 1));
		MemCopy(m_Repr.Big.Data, ptr, size);
		m_Repr.Big.Data[size] = '\0'; // StringViews are not null-terminated
		m_Repr.Big.Size = size;
		m_Repr.Big.Capacity = size;
	}
	else
	{
		SetSmall(true);
		MemCopy(m_Repr.Small.Data, ptr, size + 1);
		m_Repr.Small.Data[size] = '\0';
		m_Repr.Small.Data[m_SmallSize] = m_SmallSize - Byte(size);
	}
}

String::String(u64 size, Allocator& alloc) : m_Alloc(&alloc)
{
	if (size > m_SmallSize) // Big string
	{
		SetSmall(false);
		m_Repr.Big.Data = reinterpret_cast<Byte*>(GetAlloc()->Allocate(size + 1));
		m_Repr.Big.Size = size;
		m_Repr.Big.Capacity = size;
	}
	else
	{
		SetSmall(true);
		m_Repr.Small.Data[m_SmallSize] = m_SmallSize - Byte(size);
	}
}

String::String(const String& other) : m_Alloc(other.m_Alloc)
{
	if (other.IsSmall())
	{
		SetSmall(true);
		MemCopy(m_Repr.Small.Data, other.m_Repr.Small.Data, m_SmallSize + 1);
	}
	else
	{
		SetSmall(false);
		m_Repr.Big.Data = reinterpret_cast<Byte*>(GetAlloc()->Allocate(other.m_Repr.Big.Capacity + 1));
		m_Repr.Big.Size = other.m_Repr.Big.Size;
		m_Repr.Big.Capacity = other.m_Repr.Big.Capacity;
		MemCopy(m_Repr.Big.Data, other.m_Repr.Big.Data, m_Repr.Big.Size);
	}
}

String::String(String&& other)
{
	MemCopy(this, &other, sizeof(String));
	other.SetSmall(true);
}

String::~String()
{
	if (!IsSmall())
	{
		GetAlloc()->Deallocate(m_Repr.Big.Data);
	}
}

String& String::operator=(const String& other)
{
	Realloc(other.Size());

	if (IsSmall())
	{
		if (other.IsSmall())
		{
			MemCopy(this, &other, sizeof(String));
		}
		else
		{
			MemCopy(m_Repr.Small.Data, other.m_Repr.Big.Data, other.m_Repr.Big.Size);
			m_Repr.Small.Data[m_SmallSize] = Byte(other.m_Repr.Big.Size);
		}
	}
	else
	{
		if (other.IsSmall())
		{
			MemCopy(m_Repr.Big.Data, other.m_Repr.Small.Data, other.m_Repr.Small.Data[m_SmallSize]);
			m_Repr.Big.Size = other.m_Repr.Small.Data[m_SmallSize];
		}
		else
		{
			MemCopy(m_Repr.Big.Data, other.m_Repr.Big.Data, other.m_Repr.Big.Size);
			m_Repr.Big.Size = other.m_Repr.Big.Size;
		}
	}

	return *this;
}

String& String::operator=(String&& other)
{
	if (!IsSmall())
	{
		m_Alloc->Deallocate(m_Repr.Big.Data);
	}

	MemCopy(this, &other, sizeof(String));
	MemSet(&other, 0, sizeof(String));

	return *this;
}

String& String::operator+=(const char* ptr)
{
	*this += StringRef(ptr);

	return *this;
}

String& String::operator+=(StringRef view)
{
	u64 newSize = Size() + view.Size();
	Realloc(newSize);

	MemCopy(Data() + Size(), view.Data(), view.Size() + 1);
	IncSize(view.Size());

	return *this;
}

String String::operator+(const char* ptr)
{
	u64 ptrSize = StrLen(ptr);
	String temp(Size() + ptrSize);

	MemCopy(temp.Data(), Data(), Size());
	MemCopy(temp.Data() + Size(), ptr, ptrSize);
	temp.Data()[temp.Size()] = '\0';

	return temp;
}

String String::operator+(StringRef view)
{
	String temp(Size() + view.Size());

	MemCopy(temp.Data(), Data(), Size());
	MemCopy(temp.Data() + Size(), view.Data(), view.Size());
	temp.Data()[temp.Size()] = '\0';

	return temp;
}

String& String::Push(Char c)
{
	u64 currSize = Size();
	if (c <= 0x7f) // 1 byte
	{
		Realloc(currSize + 1);
		IncSize(1);
		Data()[currSize] = Byte(c);
		Data()[currSize + 1] = '\0';
	}
	else if (c <= 0x7ff)
	{
		Realloc(currSize + 2);
		IncSize(2);
		Data()[currSize] = 0b11000000 + ((c & 0b11111000000) >> 6);
		Data()[currSize + 1] = 0b10000000 + c & 0b111111;
		Data()[currSize + 2] = '\0';
	}
	else if (c <= 0xffff)
	{
		Realloc(currSize + 3);
		IncSize(3);
		Data()[currSize] = 0b11100000 + ((c & 0b1111000000000000) >> 12);
		Data()[currSize + 1] = 0b10000000 + ((c & 0b111111000000) >> 6);
		Data()[currSize + 2] = 0b10000000 + c & 0b111111;
		Data()[currSize + 3] = '\0';
	}
	else if (c <= 0x10ffff)
	{
		Realloc(currSize + 4);
		IncSize(4);
		Data()[currSize] = 0b11110000 + ((c & 0b111000000000000000000) >> 18);
		Data()[currSize + 1] = 0b10000000 + ((c & 0b111111000000000000) >> 12);
		Data()[currSize + 2] = 0b10000000 + ((c & 0b111111000000) >> 6);
		Data()[currSize + 3] = 0b10000000 + (c & 0b111111);
		Data()[currSize + 4] = '\0';
	}
	else
	{
		IASSERT(false, "Invalid codepoint");
	}

	return *this;
}

const Byte* String::CString() const
{
	if (IsSmall())
	{
		return m_Repr.Small.Data;
	}
	else
	{
		return m_Repr.Big.Data;
	}
}

Byte* String::Data()
{
	if (IsSmall())
	{
		return m_Repr.Small.Data;
	}
	else
	{
		return m_Repr.Big.Data;
	}
}

u64 String::Size() const
{
	if (IsSmall())
	{
		return m_SmallSize - m_Repr.Small.Data[m_SmallSize];
	}
	else
	{
		return m_Repr.Big.Size;
	}
}

void String::Reserve(u64 size) { Realloc(size); }

String::Iterator String::begin() const { return Iterator(CString()); }

String::Iterator String::end() const { return Iterator(CString() + Size()); }

bool String::IsSmall() const { return u64(m_Alloc) & 1; }

void String::SetSmall(bool isSmall)
{
	m_Alloc = reinterpret_cast<Allocator*>((u64(m_Alloc) & (u64(-1) - 1)) | Byte(isSmall));
}

void String::IncSize(u64 inc)
{
	if (IsSmall())
	{
		m_Repr.Small.Data[m_SmallSize] = m_Repr.Small.Data[m_SmallSize] - Byte(inc);
	}
	else
	{
		m_Repr.Big.Size += inc;
	}
}

Allocator* String::GetAlloc() const { return reinterpret_cast<Allocator*>(u64(m_Alloc) & (u64(-1) - 1)); }

void String::Realloc(u64 size)
{
	if (IsSmall())
	{
		if (size > m_SmallSize)
		{
			SetSmall(false);
			void* data = GetAlloc()->Allocate(size + 1);
			MemCopy(data, m_Repr.Small.Data, m_SmallSize - m_Repr.Small.Data[m_SmallSize]);
			m_Repr.Big.Data = reinterpret_cast<Byte*>(data);
			m_Repr.Big.Capacity = size;
		}
	}
	else if (size > m_Repr.Big.Capacity)
	{
		u64 capacity = m_Repr.Big.Capacity;
		while (capacity < size)
		{
			capacity *= 2;
		}

		u64 trySize = GetAlloc()->GrowAllocation(m_Repr.Big.Data, m_Repr.Big.Size, capacity + 1);
		if (trySize < size)
		{
			void* data = GetAlloc()->Allocate(capacity + 1);
			MemCopy(data, m_Repr.Big.Data, m_Repr.Big.Size);
			GetAlloc()->Deallocate(m_Repr.Big.Data);
			m_Repr.Big.Data = reinterpret_cast<Byte*>(data);
			m_Repr.Big.Capacity = capacity;
		}
		else
		{
			m_Repr.Big.Capacity = trySize;
		}
	}
}

bool operator==(const String& first, const String& second)
{
	u64 size = first.Size();
	if (size != second.Size())
	{
		return false;
	}

	if (first.IsSmall())
	{
		return MemCompare(first.m_Repr.Small.Data, second.m_Repr.Small.Data, size);
	}
	else
	{
		return MemCompare(first.m_Repr.Big.Data, second.m_Repr.Big.Data, size);
	}
}

bool operator!=(const String& first, const String& second) { return !(first == second); }

bool operator==(StringIterator first, StringIterator second) { return first.m_Byte == second.m_Byte; }

bool operator!=(StringIterator first, StringIterator second) { return first.m_Byte != second.m_Byte; }

String operator+(StringRef first, StringRef second)
{
	String str(first);
	return str + second;
}

constexpr u32 FMix32(u32 h)
{
	h ^= h >> 16;
	h *= 0x85ebca6b;
	h ^= h >> 13;
	h *= 0xc2b2ae35;
	h ^= h >> 16;

	return h;
}

u32 Rotl(u32 number, u32 places)
{
#ifdef COMPILER_MSVC
	return _rotl(number, places);
#else
	return (number << places) | (number >> (32 - places));
#endif
}

u64 Hasher<String>::Hash(const String& string)
{
	const u8* data = reinterpret_cast<const u8*>(string.CString());
	u32 blockCount = string.Size() / 4;

	u32 h1 = 192837465; // Seed
	u32 c1 = 0xcc9e2d51;
	u32 c2 = 0x1b873593;

	const u32* blocks = reinterpret_cast<const u32*>(data + blockCount * 4);
	for (i32 i = -blockCount; i; i++)
	{
		u32 k1 = blocks[i];

		k1 *= c1;
		k1 = Rotl(k1, 15);
		k1 *= c2;

		h1 ^= k1;
		h1 = Rotl(h1, 13);
		h1 = h1 * 5 + 0xe6546b64;
	}

	const u8* tail = reinterpret_cast<const u8*>(data + blockCount * 4);
	u32 k1 = 0;

	switch (string.Size() & 3)
	{
	case 3:
		k1 ^= tail[2] << 16;
	case 2:
		k1 ^= tail[1] << 8;
	case 1:
		k1 ^= tail[0];
		k1 *= c1;
		k1 = Rotl(k1, 15);
		k1 *= c2;
		h1 ^= k1;
	}

	h1 ^= string.Size();
	return FMix32(h1);
}

u64 Ignis::Hasher<StringRef>::Hash(const StringRef& string)
{
	const u8* data = reinterpret_cast<const u8*>(string.Data());
	u32 blockCount = string.Size() / 4;

	u32 h1 = 192837465; // Seed
	u32 c1 = 0xcc9e2d51;
	u32 c2 = 0x1b873593;

	const u32* blocks = reinterpret_cast<const u32*>(data + blockCount * 4);
	for (i32 i = -blockCount; i; i++)
	{
		u32 k1 = blocks[i];

		k1 *= c1;
		k1 = Rotl(k1, 15);
		k1 *= c2;

		h1 ^= k1;
		h1 = Rotl(h1, 13);
		h1 = h1 * 5 + 0xe6546b64;
	}

	const u8* tail = reinterpret_cast<const u8*>(data + blockCount * 4);
	u32 k1 = 0;

	switch (string.Size() & 3)
	{
	case 3:
		k1 ^= tail[2] << 16;
	case 2:
		k1 ^= tail[1] << 8;
	case 1:
		k1 ^= tail[0];
		k1 *= c1;
		k1 = Rotl(k1, 15);
		k1 *= c2;
		h1 ^= k1;
	}

	h1 ^= string.Size();
	return FMix32(h1);
}

}
