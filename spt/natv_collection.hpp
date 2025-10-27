
/**
 * @brief A template class for a fixed-size collection of elements.
 * @details This class provides a container for a sequence of elements of type T,
 *          along with methods for map and reduce-style operations. It manages its own memory.
 * @tparam T The type of elements in the collection.
 */

#include "../spt/assert.hpp"

#include <cstddef>
#include <functional>
#include <algorithm>
#include <iostream>
#include <numeric>
#include <iterator>

#ifndef COLLECTION_HPP
#define COLLECTION_HPP

template <typename T>
class Collection
{
public:
	/** @brief A constant forward iterator for the Collection class. */
	class const_iterator
	{
	private:
		/** @brief Reference to the collection to iterate over. */
		const Collection &_src;

		/** @brief Index to the element in the source collection currently
		 * referenced */
		std::size_t _idx;

	public:
		/**
		 * @brief Constructs a const_iterator.
		 * @param u The collection to iterate over.
		 * @param offset The starting offset within the collection.
		 */
		const_iterator(const Collection<T> &u, std::size_t offset)
			: _src(u), _idx(offset) {}

		/**
		 * @brief Dereferences the iterator to get the element.
		 * @return A constant reference to the element.
		 */
		inline T operator*() const { return _src.get(_idx); }

		/**
		 * @brief Pre-increment operator. Advances the iterator to the next element.
		 * @return A reference to the incremented iterator.
		 */
		const_iterator &operator++()
		{
			++_idx;
			return *this;
		}

		/**
		 * @brief Pre-decrement operator. Moves the iterator to the previous element.
		 * @return A reference to the decremented iterator.
		 */
		const_iterator &operator--()
		{
			--_idx;
			return *this;
		}

		/**
		 * @brief Equality comparison operator.
		 * @param itr The iterator to compare against.
		 * @return True if the iterators point to the same element, false otherwise.
		 */
		bool operator==(const const_iterator &itr)
		{
			return _src._data == itr._src._data && _idx == itr._idx;
		}

		/**
		 * @brief Inequality comparison operator.
		 * @param itr The iterator to compare against.
		 * @return True if the iterators point to different elements, false otherwise.
		 */
		bool operator!=(const const_iterator &itr)
		{
			return _src._data != itr._src._data || _idx != itr._idx;
		}
	};

private:
	/** @brief Raw pointer to the dynamically allocated array of elements. */
	T *_data;

	/** @brief The number of elements in the collection. */
	const std::size_t _size;

	/**
	 * @brief Private constructor to initialize an empty collection of a given size.
	 * @param size The number of elements to allocate space for.
	 */
	Collection(std::size_t size)
		: _data(new T[size]), _size(size) {}

public:
	/**
	 * @brief Constructs a collection by generating elements.
	 * @tparam FN The type of the generator function.
	 * @param size The number of elements to generate.
	 * @param fn A function that takes an index (std::size_t) and returns an element of type T.
	 */
	template <typename FN>
	Collection(std::size_t size, FN fn)
		: Collection(size)
	{
		std::size_t idx = 0;
		std::generate(_data, _data + _size, [&]()
					  { return fn(idx++); });
	}

	/**
	 * @brief Constructs a new collection by applying a function to each element of an existing collection (map).
	 * @tparam U The element type of the source collection.
	 * @tparam FN The type of the mapping function.
	 * @param u The source collection.
	 * @param fn A function that takes an element of type U and returns an element of type T.
	 */
	template <typename U, typename FN>
	Collection(const Collection<U> &u, FN fn)
		: Collection(u.size())
	{
		auto u_iter = u.data();
		std::generate(_data,
					  _data + _size,
					  [&]()
					  { return fn(*u_iter++); });
	}

	/**
	 * @brief Constructs a new collection by applying a binary function to elements of two existing collections (zip).
	 * @tparam U The element type of the first source collection (unused, defaults to T).
	 * @tparam V The element type of the second source collection (unused, defaults to T).
	 * @tparam FN The type of the binary function.
	 * @param u The first source collection.
	 * @param v The second source collection.
	 * @param fn A function that takes an element from u and an element from v and returns a new element of type T.
	 */
	template <typename U = T, typename V = T, typename FN>
	Collection(const Collection<T> &u, const Collection<T> &v, FN fn)
		: Collection(std::min(u.size(), v.size()))
	{
		auto u_iter = u.data();
		auto v_iter = v.data();
		std::generate(_data,
					  _data + _size,
					  [&]()
					  {
						  return fn(*u_iter++, *v_iter++);
					  });
	}

	/**
	 * @brief Move constructor. Takes ownership of the resources of another collection.
	 * @param src The source collection to move from.
	 */
	Collection(Collection<T> &&src)
		: _data(src._data), _size(src._size)
	{
		src._data = nullptr;
	}

	/** @brief Destructor. Frees the dynamically allocated memory. */
	~Collection()
	{
		if (_data != nullptr)
		{
			delete[] _data;
			_data = nullptr;
		}
	}

	/**
	 * @brief Gets the number of elements in the collection.
	 * @return The size of the collection.
	 */
	inline std::size_t size() const { return _size; }

	/**
	 * @brief Gets a pointer to the underlying data array.
	 *
	 * @return Pointer to underlying data array.
	 */
	inline const T *data() const { return _data; }

	/**
	 * @brief Copy the elements of this collection into a std::vector
	 *
	 * @return Vector containing the elements of the collection
	 */
	inline std::vector<T> to_vector() const { return std::vector<T>(_data, _data + _size); }

	/**
	 * @brief Gets the element at a specific index.
	 * @param idx The index of the element.
	 * @return A copy of the element at the specified index.
	 */
	inline T get(std::size_t idx) const
	{
		assert_true(idx < _size, "get: Index out of bounds");
		return _data[idx];
	}

	/**
	 * @brief Sets the element at a specific index.
	 * @param idx The index of the element to set.
	 * @param value The new value for the element.
	 */
	inline void set(std::size_t idx, const T &value)
	{
		assert_true(idx < _size, "set: Index out of bounds");
		_data[idx] = value;
	}

	/**
	 * @brief Gets a constant iterator to the beginning of the collection.
	 * @return A const_iterator pointing to the first element.
	 */
	inline const_iterator cbegin() const { return const_iterator(*this, 0); }

	/**
	 * @brief Gets a constant iterator to the end of the collection.
	 * @return A const_iterator pointing past the last element.
	 */
	inline const_iterator cend() const { return const_iterator(*this, _size); }

	/**
	 * @brief Reduces the collection to a single value using a binary function.
	 * @tparam FN The type of the reduction function.
	 * @param fn A binary function that takes an accumulated value and the next element, returning the new accumulated value.
	 * @return The final reduced value. Returns a default-constructed T for an empty collection.
	 */
	template <typename FN>
	T reduce(FN fn) const
	{
		if (_size == 0)
			return T(0);
		return std::accumulate(_data + 1, _data + _size, *_data, fn);
	}
};

/**
 * @brief Element-wise addition of two collections.
 * @tparam T The element type of the collections.
 * @param u The first collection.
 * @param v The second collection.
 * @return A new collection containing the element-wise sum.
 */
template <typename T>
Collection<T> operator+(const Collection<T> &u,
						const Collection<T> &v)
{
	return Collection<T>(u, v, std::plus<T>());
}

/**
 * @brief Element-wise subtraction of two collections.
 * @tparam T The element type of the collections.
 * @param u The first collection.
 * @param v The second collection.
 * @return A new collection containing the element-wise difference.
 */
template <typename T>
Collection<T> operator-(const Collection<T> &u,
						const Collection<T> &v)
{
	return Collection<T>(u, v, std::minus<T>());
}

/**
 * @brief Element-wise multiplication of two collections.
 * @tparam T The element type of the collections.
 * @param u The first collection.
 * @param v The second collection.
 * @return A new collection containing the element-wise product.
 */
template <typename T>
Collection<T> operator*(const Collection<T> &u,
						const Collection<T> &v)
{
	return Collection<T>(u, v, std::multiplies<T>());
}

/**
 * @brief Element-wise division of two collections.
 * @tparam T The element type of the collections.
 * @param u The numerator collection.
 * @param v The denominator collection.
 * @return A new collection containing the element-wise quotient.
 */
template <typename T>
Collection<T> operator/(const Collection<T> &u,
						const Collection<T> &v)
{
	return Collection<T>(u, v, std::divides<T>());
}

/**
 * @brief Calculates the sum of all elements in a collection.
 * @tparam T The element type of the collection.
 * @param u The collection to sum.
 * @return The sum of the elements.
 */
template <typename T>
T sum(const Collection<T> &u)
{
	return u.reduce(std::plus<T>());
}

/**
 * @brief Calculates the product of all elements in a collection.
 * @tparam T The element type of the collection.
 * @param u The collection.
 * @return The product of the elements.
 */
template <typename T>
T prod(const Collection<T> &u)
{
	return u.reduce(std::multiplies<T>());
}

/**
 * @brief Calculates the dot product of two collections.
 * @tparam T The element type of the collections.
 * @param u The first collection.
 * @param v The second collection.
 * @return The dot product of the two collections.
 */
template <typename T>
T dot(const Collection<T> &u, const Collection<T> &v)
{
	return sum(u * v);
}

/**
 * @brief Overloads the stream insertion operator to print a collection.
 * @tparam T The element type of the collection.
 * @param os The output stream.
 * @param u The collection to print.
 * @return The output stream.
 */
template <typename T>
std::ostream &operator<<(std::ostream &os, const Collection<T> &u)
{
	char delim = '[';
	for (auto iter = u.cbegin(); iter != u.cend(); ++iter)
	{
		os << delim << *iter;
		delim = ',';
	}
	os << ']';
	return os;
}

#endif // COLLECTION_HPP