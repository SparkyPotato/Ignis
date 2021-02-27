/// Copyright (c) 2021 Shaye Garg.
/// \file
/// Key-value pair maps.

#pragma once
#include "Core/Math/Random.h"
#include "Core/Memory/Memory.h"
#include "Core/Memory/RawAllocator.h"
#include "Core/Types/BaseTypes.h"
#include "Core/Types/Pair.h"
#include "Core/Types/Traits.h"

namespace Ignis {

template<typename>
struct Hasher;

template<typename T>
concept HashKey = std::equality_comparable<T>&& Traits::IsComplete<Hasher<T>>::value;

/// Open-addressed, quadratic-probed HashMap.
///
/// \tparam K Key type.
/// \tparam V Value type.
template<HashKey K, typename V>
class HashMap
{
private:
	struct Bucket
	{
		Pair<K, V> Slot;
		enum
		{
			Full,
			Empty,
			Tombstone
		} Status;
	};

public:
	/// Iterator over a HashMap.
	class Iterator
	{
	public:
		Iterator(Bucket* bucket, Bucket* end) : m_Bucket(bucket), m_End(end) {}

		/// Dereference the iterator.
		///
		/// \return The pair it is pointing to.
		Pair<K, V>& operator*() const { return m_Bucket->Slot; }

		/// Dereference the iterator.
		///
		/// \return The pair it is pointing to.
		Pair<K, V>* operator->() const { return &m_Bucket->Slot; }

		/// Advance the iterator.
		///
		/// \return Reference to the advanced iterator.
		Iterator& operator++()
		{
			do
			{
				m_Bucket++;
			} while (m_Bucket->Status != Bucket::Full && m_Bucket != m_End);

			return *this;
		}

		/// Advance the iterator.
		///
		/// \return The advanced iterator.
		Iterator operator++(int)
		{
			Iterator it = *this;

			do
			{
				m_Bucket++;
			} while (m_Bucket->Status != Bucket::Full && m_Bucket != m_End);

			return it;
		}

		/// Advance the iterator.
		///
		/// \param offset Number of pairs to iterate over.
		///
		/// \return Reference to the advanced iterator.
		Iterator operator+(u64 offset) const
		{
			Iterator it = *this;

			for (u64 i = 0; i < offset; i++)
			{
				it++;
			}

			return it;
		}

		/// Advance the iterator.
		///
		/// \param offset Number of pairs to iterate over.
		///
		/// \return The advanced iterator.
		Iterator& operator+=(u64 offset)
		{
			for (u64 i = 0; i < offset; i++)
			{
				(*this)++;
			}

			return *this;
		}

		friend bool operator==(Iterator first, Iterator second) { return first.m_Bucket == second.m_Bucket; }

		friend bool operator!=(Iterator first, Iterator second) { return !(first == second); }

	private:
		Bucket* m_Bucket;
		Bucket* m_End;
	};

	/// ConstIterator over a HashMap.
	class ConstIterator
	{
	public:
		ConstIterator(const Bucket* bucket, const Bucket* end) : m_Bucket(bucket), m_End(end) {}

		/// Dereference the iterator.
		///
		/// \return The pair it is pointing to.
		const Pair<K, V>& operator*() const { return m_Bucket->Slot; }

		/// Dereference the iterator.
		///
		/// \return The pair it is pointing to.
		const Pair<K, V>* operator->() const { return &m_Bucket->Slot; }

		/// Advance the iterator.
		///
		/// \return Reference to the advanced iterator.
		ConstIterator& operator++()
		{
			do
			{
				m_Bucket++;
			} while (m_Bucket->Status != Bucket::Full && m_Bucket != m_End);

			return *this;
		}

		/// Advance the iterator.
		///
		/// \return The advanced iterator.
		ConstIterator operator++(int)
		{
			Iterator it = *this;

			do
			{
				m_Bucket++;
			} while (m_Bucket->Status != Bucket::Full && m_Bucket != m_End);

			return it;
		}

		/// Advance the iterator.
		///
		/// \param offset Number of pairs to iterate over.
		///
		/// \return Reference to the advanced iterator.
		ConstIterator operator+(u64 offset) const
		{
			Iterator it = *this;

			for (u64 i = 0; i < offset; i++)
			{
				it++;
			}

			return it;
		}

		/// Advance the iterator.
		///
		/// \param offset Number of pairs to iterate over.
		///
		/// \return The advanced iterator.
		ConstIterator& operator+=(u64 offset)
		{
			for (u64 i = 0; i < offset; i++)
			{
				(*this)++;
			}

			return *this;
		}

		friend bool operator==(ConstIterator first, ConstIterator second) { return first.m_Bucket == second.m_Bucket; }

		friend bool operator!=(ConstIterator first, ConstIterator second) { return !(first == second); }

	private:
		const Bucket* m_Bucket;
		const Bucket* m_End;
	};

	/// Constructor.
	///
	/// \param alloc Allocator to use. Defaults to GAlloc.
	HashMap(Allocator& alloc = GAlloc) : m_Alloc(&alloc)
	{
		m_Capacity = 8;
		m_Mask = 7;
		m_Buckets = reinterpret_cast<Bucket*>(m_Alloc->Allocate(sizeof(Bucket) * m_Capacity));
		for (u64 i = 0; i < m_Capacity; i++)
		{
			m_Buckets[i].Status = Bucket::Empty;
		}
	}

	/// Destructor.
	~HashMap()
	{
		for (u64 i = 0; i < m_Capacity; i++)
		{
			if (m_Buckets[i].Status == Bucket::Full)
			{
				m_Buckets[i].Slot.~Pair<K, V>();
			}
		}

		m_Alloc->Deallocate(m_Buckets);
	}

	V& operator[](const K& key)
	{
		FindResult res = FindBucket(key);

		if (res.Normal && res.Normal->Status == Bucket::Full)
		{
			return res.Normal->Slot.Second;
		}

		if (float(m_Size) / m_Capacity > 0.6f)
		{
			Realloc();
			res = FindBucket(key);
		}

		auto bucket = res.Tombstone ? res.Tombstone : res.Normal;

		Construct<Pair<K, V>>(&bucket->Slot, key, V());
		bucket->Status = Bucket::Full;
		m_Size++;
		return bucket->Slot.Second;
	}

	/// Insert a key-value pair into the HashMap.
	/// If the key already exists, its value is overwritten.
	///
	/// \param key The key.
	/// \param value The value to store at the key.
	///
	/// \return Reference to the key-value pair stored.
	Pair<K, V>& Insert(const K& key, const V& value)
	{
		FindResult res = FindBucket(key);

		if (res.Normal && res.Normal->Status == Bucket::Full)
		{
			res.Normal->Slot.Second = value;
			return res.Normal->Slot;
		}

		if (float(m_Size) / m_Capacity > 0.6f)
		{
			Realloc();
			res = FindBucket(key);
		}

		auto bucket = res.Tombstone ? res.Tombstone : res.Normal;

		Construct<Pair<K, V>>(&bucket->Slot, key, value);
		bucket->Status = Bucket::Full;
		m_Size++;
		return bucket->Slot;
	}

	/// Insert a key-value pair into the HashMap.
	/// If the key already exists, its value is overwritten.
	///
	/// \param key The key.
	/// \param value The value to store at the key.
	///
	/// \return Reference to the key-value pair stored.
	Pair<K, V>& Insert(K&& key, V&& value)
	{
		FindResult res = FindBucket(key);

		if (res.Normal && res.Normal->Status == Bucket::Full)
		{
			res.Normal->Slot.Second = value;
			return res.Normal->Slot;
		}

		if (float(m_Size) / m_Capacity > 0.6f)
		{
			Realloc();
			res = FindBucket(key);
		}

		auto bucket = res.Tombstone ? res.Tombstone : res.Normal;

		Construct<Pair<K, V>>(&bucket->Slot, key, value);
		bucket->Status = Bucket::Full;
		m_Size++;
		return bucket->Slot;
	}

	/// Get the value stored at a key.
	///
	/// \param key Key to search for.
	///
	/// \return Pointer to the value. Is nullptr if the key doesn't exist.
	V* Get(const K& key)
	{
		FindResult res = FindBucket(key);

		if (res.Normal && res.Normal->Status == Bucket::Full)
		{
			return &res.Normal->Slot.Second;
		}

		return nullptr;
	}

	/// Get the value stored at a key.
	///
	/// \param key Key to search for.
	///
	/// \return Pointer to the value. Is nullptr if the key doesn't exist.
	const V* Get(const K& key) const
	{
		FindResult res = FindBucket(key);

		if (res.Normal && res.Normal->Status == Bucket::Full)
		{
			return &res.Normal->Slot.Second;
		}

		return nullptr;
	}

	/// Remove an entry from the HashMap.
	///
	/// \param key Entry to remove.
	void Remove(const K& key)
	{
		FindResult res = FindBucket(key);

		if (res.Normal && res.Normal->Status == Bucket::Full)
		{
			res.Normal->Slot.~Pair<K, V>();
			res.Normal->Status = Bucket::Tombstone;
			m_Size--;
		}
	}

	/// Get the number of entries in the HashMap.
	///
	/// \return The number of entries.
	u64 Size() { return m_Size; }

	/// Get the capacity of the HashMap before a reallocation is done.
	///
	/// \return The number of entries that can be stored until a reallocation must be done.
	u64 Capacity() { return m_Capacity; }

	/// Iteration.
	///
	/// \return Begin Iterator.
	Iterator begin()
	{
		auto bucket = m_Buckets;
		while (true)
		{
			if (bucket->Status == Bucket::Full)
			{
				return Iterator(bucket, m_Buckets + m_Capacity);
			}
			bucket++;
		}
	}

	/// Iteration.
	///
	/// \return Begin Iterator.
	ConstIterator begin() const
	{
		auto bucket = m_Buckets;
		while (true)
		{
			if (bucket->Status == Bucket::Full)
			{
				return ConstIterator(bucket, m_Buckets + m_Capacity);
			}
			bucket++;
		}
	}

	/// Iteration.
	///
	/// \return End Iterator.
	Iterator end() { return Iterator(m_Buckets + m_Capacity, m_Buckets + m_Capacity); }

	/// Iteration.
	///
	/// \return End Iterator.
	ConstIterator end() const { return ConstIterator(m_Buckets + m_Capacity, m_Buckets + m_Capacity); }

private:
	struct FindResult
	{
		Bucket* Normal = nullptr;
		Bucket* Tombstone = nullptr;
	};

	FindResult FindBucket(const K& key) const
	{
		u64 hash = Hasher<K>::Hash(key);
		u64 probe = hash & m_Mask;
		u64 i = 1;
		FindResult result;

		while (true)
		{
			if (m_Buckets[probe].Status == Bucket::Full && m_Buckets[probe].Slot.First == key)
			{
				result.Normal = m_Buckets + probe;
				return result;
			}
			else if (m_Buckets[probe].Status == Bucket::Empty)
			{
				result.Normal = m_Buckets + probe;
				return result;
			}
			else if (m_Buckets[probe].Status == Bucket::Tombstone)
			{
				result.Tombstone = m_Buckets + probe;
				if (i > m_Capacity)
				{
					return result;
				}
			}

			probe = (hash + i * i) & m_Mask;
			i++;
		}
	}

	void Realloc()
	{
		u64 oldCapacity = m_Capacity;
		m_Capacity *= 2;
		m_Mask = m_Capacity - 1;

		auto old = m_Buckets;
		m_Buckets = reinterpret_cast<Bucket*>(m_Alloc->Allocate(sizeof(Bucket) * m_Capacity));
		m_Size = 0;
		for (u64 i = 0; i < m_Capacity; i++)
		{
			m_Buckets[i].Status = Bucket::Empty;
		}

		for (u64 i = 0; i < oldCapacity; i++)
		{
			if (old[i].Status == Bucket::Full)
			{
				Insert(Move(old[i].Slot.First), Move(old[i].Slot.Second));
			}
		}

		m_Alloc->Deallocate(old);
	}

	Allocator* m_Alloc = nullptr;
	Bucket* m_Buckets = nullptr;

	u64 m_Size = 0;
	u64 m_Capacity = 0;
	u64 m_Mask = 0;
};

template<>
struct Hasher<u64>
{
	static u64 Hash(u64 value) { return value; }
};

}
