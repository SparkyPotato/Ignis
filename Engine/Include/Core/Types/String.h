/// Copyright (c) 2021 Shaye Garg.
/// \file
/// Strings and StringRefs.

#pragma once

#include "Core/Memory/RawAllocator.h"
#include "Core/Types/BaseTypes.h"

namespace Ignis {

/// Get the number of bytes taken up by a null-terminated string.
///
/// \param ptr The pointer to the first element of the string.
///
/// \return The number of bytes, EXCLUDING the null character.
u64 IGNIS_API StrLen(const char* ptr);

class IGNIS_API String;

/// Iterator over a UTF-8 character sequence.
class IGNIS_API StringIterator
{
public:
	/// Construct an iterator over a byte sequence.
	///
	/// \param b First byte of the sequence.
	StringIterator(const Byte* b);

	/// Get the codepoint at the iterator.
	///
	/// \return UTF-32 encoded codepoint.
	Char operator*() const;

	/// Advance the iterator to the next codepoint.
	///
	/// \return Reference to the advanced iterator.
	StringIterator& operator++();

	/// Advance the iterator to the next codepoint.
	///
	/// \return Iterator before advancing.
	StringIterator operator++(int);

	/// Offset the iterator by a given amount.
	///
	/// \param offset Number of codepoints to iterate over.
	///
	/// \return Iterator to the codepoint.
	StringIterator operator+(u64 offset) const;

	/// Offset the iterator by a given amount.
	///
	/// \param offset Number of codepoints to iterate over.
	///
	/// \return Iterator to the codepoint.
	StringIterator& operator+=(u64 offset);

private:
	friend bool IGNIS_API operator==(StringIterator first, StringIterator second);
	friend bool IGNIS_API operator!=(StringIterator first, StringIterator second);

	const Byte* m_Byte;
};

bool IGNIS_API operator==(StringIterator first, StringIterator second);
bool IGNIS_API operator!=(StringIterator first, StringIterator second);

/// Non-owning view into a String or a UTF-8 encoded string literal.
class IGNIS_API StringRef
{
public:
	/// Default constructor.
	StringRef() = default;

	/// Construct a StringRef from a string literal.
	/// Use StringView(const char* ptr, uint64 size) if you already know the size.
	///
	/// \param ptr Pointer to the first character of the literal.
	StringRef(const char* ptr);

	/// Construct a StringRef from a pointer with a size.
	///
	/// \param ptr Pointer to the first character of the literal.
	/// \param size Size of the region in bytes.
	StringRef(const char* ptr, u64 size);

	/// Construct a StringRef from a String. May be invalidated on appending to the String.
	/// To prevent invalidation, first call Reserve on the String to ensure enough capacity exists.
	///
	/// \param string The String to take a view of.
	StringRef(const String& string);

	/// Append a StringRef.
	///
	/// \param ref StringRef to append.
	/// 
	/// \return String containing the concatenated string.
	String operator+(StringRef ref) const;

	/// Get the size of the view into the string.
	///
	/// \return The size.
	u64 Size() const;

	/// Get the data the view points to.
	///
	/// \return Pointer to the first character of the view.
	const Byte* Data() const;

	using Iterator = StringIterator;

	/// Iteration.
	///
	/// \return Iterator to the first character of the string.
	Iterator begin() const;

	/// Iteration.
	///
	/// \return Beyond the end iterator.
	Iterator end() const;

private:
	const Byte* m_Data = nullptr;
	u64 m_Size = 0;
};

/// Owning String with small-string optimization for storage of 23 characters.
class IGNIS_API String
{
public:
	/// Construct a String with an allocator.
	///
	/// \param alloc Allocator to use for memory allocation. Defaults to the Raw Allocator.
	String(Allocator& alloc = GAlloc);

	/// Create a String from a string literal.
	///
	/// \param ptr Pointer to first character of string literal.
	/// \param alloc Allocator to use for memory allocation. Defaults to the Raw Allocator.
	String(const char* ptr, Allocator& alloc = GAlloc);

	/// Create a String from a StringRef.
	///
	/// \param ref StringRef to create the String from.
	/// \param alloc Allocator to use for memory allocation. Defaults to the Raw Allocator.
	String(StringRef ref, Allocator& alloc = GAlloc);

	/// Create a String with a size.
	///
	/// \param size The size of the String.
	/// \param alloc Allocator to use for memory allocation. Defaults to the Raw Allocator.
	///
	/// \warning This should only be used if you are writing directly to the String's buffer with Data().
	String(u64 size, Allocator& alloc = GAlloc);

	/// Copy constructor.
	///
	/// \param other String to copy.
	String(const String& other);

	/// Move constructor.
	///
	/// \param other String to move.
	String(String&& other);

	/// Destructor.
	~String();

	/// Copy assignment.
	///
	/// \param other String to copy.
	String& operator=(const String& other);

	/// Move assignment.
	///
	/// \param other String to move.
	String& operator=(String&& other);

	/// Append a string literal.
	///
	/// \param ptr Pointer to first character of string literal to append.
	String& operator+=(const char* ptr);

	/// Append a StringView.
	///
	/// \param view StringView to append.
	String& operator+=(StringRef view);

	/// Append a string literal.
	///
	/// \param ptr Pointer to first character of string literal to append.
	String operator+(const char* ptr);

	/// Append a StringView.
	///
	/// \param view StringView to append.
	String operator+(StringRef view);

	/// Append a UTF-32 codepoint into the String.
	///
	/// \param c The codepoint to append.
	///
	/// \return The String with the appended character.
	String& Push(Char c);

	/// Get the data of the string as bytes.
	///
	/// \return Null-terminated string of bytes.
	const Byte* CString() const;

	/// Get the data of the string as bytes.
	///
	/// \return Null-terminated string of bytes.
	Byte* Data();

	/// Get the number of bytes in the string, excluding the null-terminator.
	///
	/// \return The number of bytes.
	u64 Size() const;

	/// Reserve space in the string.
	///
	/// \param size The number of bytes to reserve (+1 for the null terminator).
	void Reserve(u64 size);

	using Iterator = StringIterator;

	/// Iteration.
	///
	/// \return Iterator to the first character of the string.
	Iterator begin() const;

	/// Iteration.
	///
	/// \return Beyond the end iterator.
	Iterator end() const;

private:
	friend bool IGNIS_API operator==(const String& first, const String& second);
	friend bool IGNIS_API operator != (const String& first, const String& second);

	bool IsSmall() const;
	void SetSmall(bool isSmall);
	void IncSize(u64 inc);
	Allocator* GetAlloc() const;
	void Realloc(u64 size);

	// First bit of m_Alloc stores if the string is a small string.
	Allocator* m_Alloc;

	static constexpr u64 m_SmallSize = 23;

	// Last byte of Small.Data stores number of bytes of space left. It becomes the null terminator when it is full.
	union Repr
	{
		Repr();

		struct
		{
			Byte Data[m_SmallSize + 1];
		} Small;

		struct
		{
			u64 Size = 0;	  // Excluding terminating null.
			u64 Capacity = 0; // Excluding terminating null.
			Byte* Data = nullptr;
		} Big;
	} m_Repr;
};

bool IGNIS_API operator ==(const String& first, const String& second);
bool IGNIS_API operator !=(const String& first, const String& second);

}
