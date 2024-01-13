#pragma once
#ifndef SLR_CONTAINERS_DYNAMICARRAY
#define SLR_CONTAINERS_DYNAMICARRAY

#include <utility>

#include "SlrLib/Containers/Conformance.hpp"
#include "SlrLib/Containers/Iterator.hpp"
#include "SlrLib/Internal/Namespace.hpp"
#include "SlrLib/Memory/Allocation.hpp"
#include "SlrLib/Utilities/Types.hpp"

SLR_NAMESPACE_BEGIN

/**
* 
*/
template<typename _Type>
class DynamicArray
{
public:
	/**
	* Default constructor
	*/
	DynamicArray() = default;

	/**
	* Destructor
	* Remove all elements and delete the allocation for the buffer
	*/
	~DynamicArray()
	{
		// Call the destructor for all elements
		Status removeAllStatus = RemoveAll();
		SLR_ERROR(removeAllStatus == Status::SUCCESS, "Could not remove all elements");

		// Delete the allocation for the buffer
		Status deleteAllocationStatus = DeleteAllocation();
		SLR_ERROR(deleteAllocationStatus == Status::SUCCESS, "Could not deallocate buffer");
	}

	/**
	* Appends an element, by rvalue, to the end of the array
	* This will increase the capacity if necessary
	*/
	Status Add(_Type&& _value)
	{
		// Get the number of available elements
		const size availableCapacity = capacity - elements;

		// If there isn't enough room to add a new element
		if (availableCapacity < 1)
		{
			// Increase the capacity to account for the new element being added
			Status statusExtendCapacity = this->ExpandCapacity();
			SLR_ASSERT_ERROR(statusExtendCapacity == Status::SUCCESS, "Could not expand capacity")
			{
				return Status::FAIL;
			}
		}

		// Insert the new element in-place of the next available index
		new(&buffer[elements]) _Type(std::move(_value));

		// Increment the elements count
		++elements;

		return Status::SUCCESS;
	}

	/**
	* Appends an element to the end of the array
	* This will increase the capacity if necessary
	*/
	Status Add(const _Type& _value) 
	{
		// Get the number of available elements
		const size availableCapacity = capacity - elements;

		// If there isn't enough room to add a new element
		if (availableCapacity < 1)
		{
			// Increase the capacity to account for the new element being added
			Status statusExtendCapacity = this->ExpandCapacity();
			SLR_ASSERT_ERROR(statusExtendCapacity == Status::SUCCESS, "Could not expand capacity")
			{
				return Status::FAIL;
			}
		}

		// Insert the new element in-place of the next available index
		new(&buffer[elements]) _Type(_value);

		// Increment the elements count
		++elements;

		return Status::SUCCESS;
	}

	/**
	* Inserts an element at a given index
	* The provided index must be equal-to-or-greater-than the current number of elements
	*/
	Status Insert(const _Type& _value, const size _index)
	{
		SLR_ASSERT_ERROR(_index <= elements, "Invalid index provided to insert at")
		{
			return Status::FAIL;
		}

		// If the user wants to add it at the end of the buffer
		if (_index == this->elements)
		{
			// Call the add function
			// This requires less computation than inserting using the method below
			Status statusAdd = this->Add(_value);
			SLR_ASSERT_ERROR(statusAdd == Status::SUCCESS, "Could not append element to array")
			{
				return Status::FAIL;
			}
		}
		// Else the user wants to add it somewhere in the middle of the buffer
		else
		{
			// Get the available capacity of the buffer
			const size availableCapacity = this->capacity - this->elements;

			// If there is no more capacity to insert a new element
			if (availableCapacity < 1)
			{
				// Increase the capacity to account for the new element being added
				Status statusExtendCapacity = this->ExpandCapacity();
				SLR_ASSERT_ERROR(statusExtendCapacity == Status::SUCCESS, "Could not expand capacity")
				{
					return Status::FAIL;
				}
			}

			// Go through each element from the end of the array to the index the user wants to insert at
			for (size insertionIndex = this->elements; insertionIndex > _index; --insertionIndex)
			{
				// Set the current insertion index to the previous element
				new(&buffer[insertionIndex]) _Type(std::move(buffer[insertionIndex - 1]));
			}

			// Set the new value at the given index
			new(&buffer[_index]) _Type(std::move(_value));

			// Increment the number of elements contained within the array
			++this->elements;
		}

		return Status::SUCCESS;
	}

	/**
	* Remove an element from the array by index and adjusts the other elements to maintain a consecutive order
	* This will call the destructor for the object
	* If _index >= elements, FAIL will be returned and no element will be removed
	*/
	Status Remove(const size _index)
	{
		SLR_ASSERT_ERROR(_index < elements, "Provided index is out-of-range")
		{
			return Status::FAIL;
		}

		// Destruct the object at _index
		buffer[_index].~_Type();

		// Move all elements from the right of _index to the left, overwriting the element at _index, effectively removing
		// it from the array
		for (size moveIndex = _index; moveIndex < this->elements - 1; ++moveIndex)
		{
			// Move the element to the right to the current index
			new(&buffer[moveIndex]) _Type(std::move(buffer[moveIndex + 1]));
		}

		// Decrement the number of elements
		--this->elements;

		return Status::SUCCESS;
	}

	/**
	* Calls the constructor for all elements and removes all elements from the array
	*/
	Status RemoveAll()
	{
		// Go through each element within the buffer
		for (size index = 0; index < this->elements; ++index)
		{
			// Call the destructor for the element
			buffer[index].~_Type();
		}

		// Set the element count to zero
		this->elements = 0;

		return Status::SUCCESS;
	}

	/**
	* Shrinks the capacity of the buffer to match the number of existing elements
	*/
	Status FitCapacityToElements()
	{
		// Set the capacity to the number of elements which exist
		SetCapacity(this->elements);

		return Status::SUCCESS;
	}

	/**
	* Sets the number of elements which can be contained within the buffer
	* If _elements is less than the current number of elements, the trailing elements will be discarded to match the new
	* capacity; this will not call the destructor for the elements.
	* It is valid for the capacity to be set to zero
	* If `elements` is less than the new capacity, `elements` will be set to the new capacity
	*/
	Status SetCapacity(const size _elements)
	{
		// If the desired capacity is different to the current capacity
		if (_elements != capacity)
		{
			// If the user wants to set the capacity to zero
			if (_elements == 0)
			{
				// Delete the buffer which consequently sets the capacity to zero
				Status status = DeleteAllocation();

				SLR_ASSERT_ERROR(status == Status::SUCCESS, "Could not deallocate buffer")
				{
					return Status::FAIL;
				}
			}
			// Otherwise set the new buffer size
			else
			{
				// Get the initial amount of elements prior to reallocating so we can calculate how many elements were
				// discarded (if any)
				const size initialElementsCount = elements;

				Status status;

				// If no buffer currently exists
				if (buffer == nullptr)
				{
					// Create a new buffer
					status = MemAlloc<_Type>(buffer, _elements * elementSize);
				}
				// Otherwise there is an existing buffer which needs reallocated
				else
				{
					// Reallocate buffer to new size
					status = MemRealloc<_Type>(buffer, _elements * elementSize);
				}

				SLR_ASSERT_ERROR(status == Status::SUCCESS, "Could not (re)allocate buffer")
				{
					return Status::FAIL;
				}

				// Set the new number of elements
				capacity = _elements;

				// If the amount of elements has reduced
				if (capacity < initialElementsCount)
				{
					// Calculate how many elements were removed
					elements = initialElementsCount - (initialElementsCount - capacity);
				}
			}
		}

		return Status::SUCCESS;
	}

	/**
	* Returns whether the array contains a given element or not
	*/
	Status Contains(SLR_RETURN(bool) _doesContain, const _Type& _value)
	{
		// Start by stating it does not contain the element
		// If no element if found which matches _value, this will remain as false
		_doesContain = false;

		// Go through each element within the buffer
		for (size index = 0; index < this->elements; ++index)
		{
			// If the element is equal to the one provided
			if (buffer[index] == _value)
			{
				// Set the flag to state the container contains this item
				_doesContain = true;

				// We know it already contains the element, so no need to keep searching
				break;
			}
		}

		return Status::SUCCESS;
	}

	/**
	* Returns the number of elements stored within the array
	*/
	inline Status GetSize(SLR_RETURN(size) _size)
	{
		_size = this->elements;

		return Status::SUCCESS;
	}

	/**
	* Returns the capacity of the array
	*/
	inline Status GetCapacity(SLR_RETURN(size) _capacity)
	{
		_capacity = this->capacity;

		return Status::SUCCESS;
	}

private:
	/**
	* A pointer to the containing the dynamic array
	* Nullptr signifies that the buffer is empty and contains no elements, therefore, the assumption that buffer is not
	* nullptr cannot be made.
	*/
	_Type* buffer = nullptr;

	/**
	* The number of elements which is contained within the buffer by the user
	*/
	size elements = 0;

	/**
	* The total capacity, in number of elements, which is reserved for the buffer
	*/
	size capacity = 0;

	/**
	* The size of each element in bytes
	*/
	static const constexpr size elementSize = sizeof(_Type);

	/**
	* The rate at which the capacity increases when a reallocation is required
	* For example, with a growth factor of 1.4, an initial capacity of 6 would increase to a capacity of 8, then 1 will
	* always be added, that means:
	*   newCapacity = floor(initialCapacity * growthFactor) + 1
	*   newCapacity = floor(6 * 1.4) + 1
	*   newCapacity = floor(8.4) + 1
	*   newCapacity = 8 + 1
	*   newCapacity = 9
	*/
	static const constexpr double growthFactor = 1.4;

	/**
	* Deletes the buffer and sets `capacity` and `elements` to 0
	* This also assigns `buffer` to nullptr
	* This does not call the destructor for the contained elements
	* You may call this function even if `buffer` is currently nullptr
	*/
	inline Status DeleteAllocation()
	{
		if (buffer != nullptr)
		{
			// Delete the allocation
			Status status = MemFree<_Type>(buffer);
			SLR_ASSERT_ERROR(status == Status::SUCCESS, "Could not free allocation for dynamic container")
			{
				return Status::FAIL;
			}

			// Set the buffer to be nullptr to signify there are no contained elements
			buffer = nullptr;

			// No capacity and no elements
			this->capacity = 0;
			this->elements = 0;
		}

		return Status::SUCCESS;
	}

	/**
	* This will increase the capacity of the buffer accounting for the growth rate
	* Even if there is sufficient capacity, this will still increase, therefore a check for if the call is necessary must be
	* made by the caller
	*/
	inline Status ExpandCapacity()
	{
		// Extend the capacity of the buffer
		Status status = SetCapacity((size)(this->capacity * this->growthFactor) + 1);
		SLR_ASSERT_ERROR(status == Status::SUCCESS, "Could not set capacity")
		{
			return Status::FAIL;
		}

		return Status::SUCCESS;
	}
};

SLR_NAMESPACE_END

#endif // ifndef SLR_CONTAINERS_DYNAMICARRAY