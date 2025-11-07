/**
 * @file cuda_collection.cuh
 * @brief Defines the Collection class and related free functions for use in CUDA projects.
 */

#ifndef COLLECTION_CUH
#define COLLECTION_CUH

#include "../spt/assert.hpp"

#include <cstddef>
#include <functional>
#include <algorithm>
#include <iostream>
#include <numeric>
#include <string>
#include <iterator>
#include <cstdio>

/**
 * @brief Global function to use a function to initialize the elements of
 * a kernel array.
 * @tparam T Type of values the resulting array will contain
 * @tparam FN Function type transforming the index of the array element
 * into a value for that element.
 * @param size Number of elements in the array to initialize
 * @param result Array in the device kernel to be initialized
 * @param fn Function to map the array index to a value to assign the
 * corresponding array element.
 */
template <typename T, typename FN>
__global__ void cuda_init(std::size_t size, T *result, FN fn)
{
	std::size_t idx = blockDim.x * blockIdx.x + threadIdx.x;

	if (idx < size)
	{
		result[idx] = fn(idx);
	}
}

/**
 * @brief Global function to use a function to map the values of one array
 * onto another array
 * @tparam T Type of values the resulting array will contain
 * @tparam U Type of values the source array contains
 * @tparam FN Function type transforming the index of the array element
 * into a value for that element.
 * @param size Number of elements in the array to initialize
 * @param result Array in the device kernel to be initialized
 * @param u Array containing elements to map to the destination array
 * @param fn Function to map the array index to a value to assign the
 * corresponding array element.
 */
template <typename T, typename U, typename FN>
__global__ void cuda_map(std::size_t size, T *result, const U *u, FN fn)
{
	std::size_t idx = blockDim.x * blockIdx.x + threadIdx.x;

	if (idx < size)
		result[idx] = fn(u[idx]);
}

/**
 * @brief Global function which performs an operation on two arrays in the
 * kernel and places the results into another array in the kernel.
 * @tparam T Type of the resulting values when the operation is performed
 * on the operands
 * @tparam U Type of the "left" side operand
 * @tparam V Type of the "right" side operand
 * @tparam FN function while returns an argument of type T and takes
 * arguments of type L, R, std::size_t in that order
 * @param size Number of elements in each of the arrays given
 * @param res Array in the device kernel where the results will be placed
 * @param u Array in the device kernel where the "left" operands are
 * located
 * @param V Array in the device kernel where the "right" operands are
 * located
 * @param fn Function to map values of types L and R into type T in
 * conjunction with the array index
 */
template <typename T, typename U, typename V, typename FN>
__global__ void cuda_zip(
	std::size_t size,
	T *result,
	const U *u,
	const V *v,
	FN fn)
{
	std::size_t idx = blockDim.x * blockIdx.x + threadIdx.x;

	if (idx < size)
		result[idx] = fn(u[idx], v[idx]);
}

/**
 * @brief Global function which performs a single step in a reduce
 * operation on a function.
 * @details A single call to this function will perform the operation on
 * elements i and i+s where s is half the size given.  The result will be
 * stored in element i.
 * @tparam T Type of values in the array
 * @tparam FN Function type taking two arguments ot type T and returning a single value of type T.  This function should be commutative.
 * @param size Number of elements in the array to reduce
 * @param result Array where the results of this step will be held
 * @param src Array where the source arguments are stored
 * @param fn Function to map two arguments of type T to a single argument of type T. The function fn should be commutative
 */
template <typename T, typename FN>
__global__ void cuda_step(std::size_t size, T *result, const T *src, FN fn)
{
	std::size_t idx = blockDim.x * blockIdx.x + threadIdx.x;
	std::size_t span = (size / 2) + (size % 2);
	std::size_t pair_idx = idx + span;

	if (idx < span)
		result[idx] = (pair_idx < size) ? fn(src[idx], src[pair_idx]) : src[idx];
}

/**
 * @brief A template class for a fixed-size collection of elements.
 * @details This class provides a container for a sequence of elements of type T,
 *          along with methods for map and reduce-style operations. It manages its own memory.
 * @tparam T The type of elements in the collection.
 */
template <typename T>
class Collection
{
public:
	/** @brief A constant forward iterator for the Collection class. */
	class const_iterator
	{
	private:
		/** @brief Pointer to the current element in the iteration. */
		const Collection &_src;

		/** @brief Index of the element being iterated over.*/
		std::size_t _idx;

	public:
		/**
		 * @brief Constructs a const_iterator.
		 * @param u The collection to iterate over.
		 * @param offset The starting offset within the collection.
		 */
		const_iterator(const Collection<T> &u, std::size_t offset) : _src(u), _idx(offset) {}

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

	/** @brief Size of the CUDA block to use for these operations */
	const static std::size_t THREADS_PER_BLOCK = 1024;

	/** @brief Number of blocks neede to process this vector */
	const std::size_t BLOCK_COUNT;

	/**
	 * @brief Return the ceiling of the unsigned integer division when the
	 * numerator is divided by the denominator
	 * @param n Numerator
	 * @param d Divisor
	 * @return Ceiling of the unsigned integer division n / d.
	 */
	static std::size_t div_ceil(std::size_t n, std::size_t d)
	{
		return (n / d) + (n % d > 0);
	}

	/**
	 * @brief Similar to div_ceil, but optimized for the case where the divisor
	 * is 2.
	 * @param n Numerator
	 * @return Ceiling of n / 2
	 */
	static std::size_t half_ceil(std::size_t n)
	{
		return (n / 2) + (n % 2);
	}

	/**
	 * @brief Private constructor to initialize an empty collection of a given size.
	 * @param size The number of elements to allocate space for.
	 */
	Collection(std::size_t size)
		: _data(nullptr),
		  _size(size),
		  BLOCK_COUNT(div_ceil(size, THREADS_PER_BLOCK))
	{
		cudaMalloc(&_data, _size * sizeof(T));
		report_error("Collection private constructor: cudaMalloc failed");
	}

public:
	/**
	 * @brief Report any errors that may have occured
	 * @param location A string identifying the location where the error check is being performed.
	 */
	static void report_error(const std::string &location)
	{
		// Check for asynchronous errors from kernel launches or other async calls.
		// This call also clears the error state.
		cudaError_t err = cudaGetLastError();
		if (err != cudaSuccess)
		{
			std::cerr << location << ": Asynchronous error: " << cudaGetErrorString(err) << std::endl;
			std::exit(EXIT_FAILURE);
		}

		// Wait for the kernel to complete and check for execution errors
		err = cudaDeviceSynchronize();
		if (err != cudaSuccess)
		{
			std::cerr << location << ": Kernel execution error: " << cudaGetErrorString(err) << std::endl;
			std::exit(EXIT_FAILURE);
		}
	}

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
		cuda_init<<<BLOCK_COUNT, THREADS_PER_BLOCK>>>(_size, _data, fn);
		report_error("Collection(size, fn) constructor");
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
		cuda_map<<<BLOCK_COUNT, THREADS_PER_BLOCK>>>(_size, _data, u.data(), fn);
		report_error("Collection map constructor");
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
		cuda_zip<<<BLOCK_COUNT, THREADS_PER_BLOCK>>>(_size, _data, u.data(), v.data(), fn);
		report_error("Collection zip constructor");
	}

	/**
	 * @brief Move constructor. Takes ownership of the resources of another collection.
	 * @param src The source collection to move from.
	 */
	Collection(Collection<T> &&src)
		: _data(src._data),
		  _size(src._size),
		  BLOCK_COUNT(src.BLOCK_COUNT)
	{
		src._data = nullptr;
	}

	/** @brief Destructor. Frees the dynamically allocated memory. */
	~Collection()
	{
		if (_data != nullptr)
		{
			// During stack unwinding from an exception, we must not throw another
			// exception from the destructor, as this would call std::terminate().
			// We check for and clear any "sticky" errors from a previous failure,
			// then call cudaFree. If cudaFree itself fails, we report the error
			// to stderr but do not throw, allowing the original exception to be handled.
			cudaError_t err = cudaGetLastError(); // Clear sticky errors
			if (err != cudaSuccess)
			{
				std::cerr << "CUDA sticky error in ~Collection(): " << cudaGetErrorString(err) << std::endl;
			}

			err = cudaFree(_data);
			if (err != cudaSuccess)
			{
				std::cerr << "cudaFree failed in ~Collection(): " << cudaGetErrorString(err) << std::endl;
			}
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
	 * @return A pointer to the data array.
	 */
	inline const T *data() const { return _data; }

	/**
	 * @brief Copy the elements of this collection into a std::vector
	 *
	 * @return Vector containing the elements of the collection
	 */
	inline std::vector<T> to_vector() const
	{
		std::vector<T> local_data(_size);
		assert_equal(
			cudaMemcpy(local_data.data(), _data, _size * sizeof(T), cudaMemcpyDeviceToHost), cudaSuccess,
			"to_vector: cudaMemcpy failed");
		return local_data;
	}

	/**
	 * @brief Gets the element at a specific index.
	 * @param idx The index of the element.
	 * @return A copy of the element at the specified index.
	 */
	inline T get(std::size_t idx) const
	{
		assert_true(idx < _size, "get: Index out of bounds");
		T return_value;
		assert_equal(cudaMemcpy(&return_value, _data + idx, sizeof(T), cudaMemcpyDeviceToHost), cudaSuccess, "get: cudaMemcpy failed");

		return return_value;
	}

	/**
	 * @brief Sets the element at a specific index.
	 * @param idx The index of the element to set.
	 * @param value The new value for the element.
	 */
	inline void set(std::size_t idx, const T &value)
	{
		assert_true(idx < _size, "set: Index out of bounds");
		assert_equal(cudaMemcpy(_data + idx, &value, sizeof(T), cudaMemcpyHostToDevice), cudaSuccess, "set: cudaMemcpy failed");
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
		std::size_t size = _size;
		std::size_t next_size = half_ceil(size);
		T *interim_result;
		assert_equal(cudaMalloc(&interim_result, next_size * sizeof(T)), cudaSuccess, "reduce: cudaMalloc failed");
		T *current_src = _data;

		while (size > 1)
		{
			std::size_t num_blocks = div_ceil(size, THREADS_PER_BLOCK);

			cuda_step<<<num_blocks, THREADS_PER_BLOCK>>>(size, interim_result, current_src, fn);
			report_error("reduce step");

			size = next_size;
			next_size = half_ceil(size);
			// The result of this step is the source for the next
			current_src = interim_result;
		}

		T return_value;
		assert_equal(cudaMemcpy(&return_value, interim_result, sizeof(T), cudaMemcpyDeviceToHost), cudaSuccess, "reduce: final cudaMemcpy failed");
		assert_equal(cudaFree(interim_result), cudaSuccess, "reduce: cudaFree failed");

		return return_value;
	}

	/**
	 * @brief Creates a new collection by applying a function to each element of this collection.
	 * @details This method iterates through each element of the current collection, applies the
	 *          provided function `fn` to it, and stores the result in a new collection.
	 *          This is a classic "map" operation from functional programming.
	 * @tparam U The element type of the new collection.
	 * @tparam FN The type of the mapping function.
	 * @param fn A function that takes an element of type T (the current collection's type)
	 *           and returns an element of type U (the new collection's type).
	 * @return A new `Collection<U>` containing the transformed elements.
	 */
	template <typename U, typename FN>
	Collection<U> map(FN fn) const
	{
		return Collection<U>(*this, fn);
	}

	/**
	 * @brief Creates a new collection by combining elements from this collection and another using a binary function.
	 * @details This method pairs up elements from the current collection (`this`) and the provided
	 *          collection `v`, applies the binary function `fn` to each pair, and stores the
	 *          result in a new collection. This is a classic "zip" operation. The resulting
	 *          collection will have a size equal to the minimum of the two source collections' sizes.
	 * @tparam U The element type of the new collection.
	 * @tparam V The element type of the second source collection.
	 * @tparam FN The type of the binary function.
	 * @param v The second collection to zip with.
	 * @param fn A binary function that takes an element from `this` (type T) and an element
	 *           from `v` (type V) and returns an element for the new collection (type U).
	 * @return A new `Collection<U>` containing the combined elements.
	 */
	template <typename U, typename V, typename FN>
	Collection<U> zip(const Collection<V> &v, FN fn) const
	{
		return Collection<U>(*this, v, fn);
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
	return Collection<T>(u, v, [] __device__(T x, T y)
						 { return x + y; });
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
	return Collection<T>(u, v, [] __device__(T x, T y)
						 { return x - y; });
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
	return Collection<T>(u, v, [] __device__(T x, T y)
						 { return x * y; });
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
	return Collection<T>(u, v, [] __device__(T x, T y)
						 { return x / y; });
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
	return u.reduce([] __device__(T x, T y)
					{ return x + y; });
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
	return u.reduce([] __device__(T x, T y)
					{ return x * y; });
}

/**
 * @brief Calculates the dot product of two collections.
 * @tparam T The element type of the collections.
 * @param u The first collection.
 * @param v The second collection.
 * @return The dot product of the two collections.
 */
template <typename T>
T dot(const Collection<T> &u,
	  const Collection<T> &v)
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
	std::vector<T> local_data = u.to_vector();

	char delim = '[';

	for (const auto &elem : local_data)
	{
		os << delim << elem;
		delim = ',';
	}
	os << ']';

	return os;
}

#endif // COLLECTION_CUH