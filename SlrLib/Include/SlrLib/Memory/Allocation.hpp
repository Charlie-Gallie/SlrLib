#pragma once
#ifndef SLR_MEMORY_ALLOCATION
#define SLR_MEMORY_ALLOCATION

#include <cstdlib>

#include "SlrLib/ErrorHandling/Exception.hpp"
#include "SlrLib/Internal/Namespace.hpp"
#include "SlrLib/Utilities/Macros.hpp"
#include "SlrLib/Utilities/Types.hpp"

SLR_NAMESPACE_BEGIN

/**
* Dynamically allocates memory
* This will allocate _bytes bytes and assign _allocation to the address of the first byte allocated
* The template is only used for ease of conversions; it does not affect the quantity of bytes allocated
* Memory allocated via this function must be freed with MemFree(...), opposed to std::free, delete, etc.
* Attempting to allocate 0 bytes will return Status::FAIL
* Under the circumstance Status::FAIL is returned, the value of _allocation should be disregarded
* The amount of bytes allocated is stored prior to the allocation itself as type `size`:
*     [ allocation size in bytes ][ allocation ]
* Use MemSize(...) to access this value
*/
template<typename _Type = void>
Status MemAlloc(SLR_RETURN(_Type*) _allocation, const size _bytes)
{
	SLR_ASSERT_ERROR(_bytes != 0, "Attempted to allocate 0 bytes")
	{
		return Status::FAIL;
	}

	// Allocate bytes to store memory buffer size too
	size bytesToAllocate = _bytes + sizeof(size);

	// Allocate buffer
	void* allocation = std::malloc(bytesToAllocate);
	SLR_ASSERT_ERROR(allocation != nullptr, "Failed to allocate memory")
	{
		return Status::FAIL;
	}

	// Set the amount of bytes allocated
	*static_cast<size*>(allocation) = _bytes;

	// Assign the return value of the allocation
	_allocation = reinterpret_cast<_Type*>((size)allocation + sizeof(size));

	return Status::SUCCESS;
}

/**
* Reallocates an existing allocation, allocated by MemAlloc(...) or MemRealloc(...), to match _bytes
* If the new size of the allocation is less than the current allocation, the trailing bytes will be discarded
* If the new number of bytes is equal to the previous number of bytes, no call has no affect
* Attempting to reallocate an allocation which was not previously allocated with MemAlloc(...) or MemRealloc(...) will
* result in undefined behavior.
* If the function call returns FAIL, the original pointer provided will remain valid but will be the original size
* The new number of bytes must be greater than 0 otherwise the function call will fail
*/
template<typename _Type = void>
Status MemRealloc(SLR_RETURN(_Type*) _allocation, const size _bytes)
{
	SLR_ASSERT_ERROR(_allocation != nullptr, "Cannot reallocate a nullptr")
	{
		return Status::FAIL;
	}

	SLR_ASSERT_ERROR(_bytes > 0, "Attempted to set allocation size to 0")
	{
		return Status::FAIL;
	}

	// We reserve the memory to store the allocation size too
	const size newAllocationSize = _bytes + sizeof(size);

	// Get a pointer to the allocation
	void* allocationPointer = reinterpret_cast<void*>((size)_allocation - sizeof(size));

	// Create a new allocation
	void* newAllocation = std::realloc(allocationPointer, newAllocationSize);
	SLR_ASSERT_ERROR(newAllocation != nullptr, "Could not reallocate memory")
	{
		return Status::FAIL;
	}

	// Set the new size of the allocation
	*static_cast<size*>(newAllocation) = _bytes;

	// Get the offset to the allocation for the user
	_allocation = reinterpret_cast<_Type*>((size)newAllocation + sizeof(size));

	return Status::SUCCESS;
}

/**
* Frees memory allocated with MemAlloc(...) or MemRealloc(...)
* Once the memory is freed, _allocation is set to nullptr
* If _allocation is a nullptr, a warning is logged
*/
template<typename _Type = void>
Status MemFree(SLR_RETURN(_Type*) _allocation)
{
	SLR_ASSERT_WARNING(_allocation != nullptr, "Attempted to free a nullptr")
	{
		return Status::FAIL;
	}

	// Get a pointer to where the actual allocation was
	void* allocationPointer = reinterpret_cast<void*>((size)_allocation - sizeof(size));

	// Free the allocation
	std::free(allocationPointer);

	// Assign the pointer of the allocation to nullptr
	_allocation = nullptr;

	return Status::SUCCESS;
}

/**
* Returns the amount of bytes allocated by MemAlloc(...) or MemRealloc(...) for a given allocation
* Providing a pointer which was not allocated by MemAlloc(...) or MemRealloc(...) results in undefined behavior
*/
template<typename _Type = void>
Status MemSize(SLR_RETURN(size) _bytes, const _Type* _allocation)
{
	// Get a const void pointer to _allocation
	const void* allocationPointer = static_cast<const void*>(_allocation);

	// Get a pointer to the allocation size
	const size* allocationSize = reinterpret_cast<size*>((size)allocationPointer - sizeof(size));

	// Set the return value for the allocation size
	_bytes = *allocationSize;

	return Status::SUCCESS;
}

SLR_NAMESPACE_END

#endif // ifndef SLR_MEMORY_ALLOCATION