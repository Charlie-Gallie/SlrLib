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
Status MemAlloc(const size _bytes, SLR_RETURN(_Type*) _allocation)
{
	SLR_ASSERT_ERROR(_bytes != 0, INVALID_SIZE, "Attempted to allocate 0 bytes");

	// Allocate bytes to store memory buffer size too
	size bytesToAllocate = _bytes + sizeof(size);

	// Allocate buffer
	void* allocation = std::malloc(bytesToAllocate);
	SLR_ASSERT_ERROR(allocation != nullptr, COULD_NOT_ALLOCATE, "Failed to allocate memory");

	// Set the amount of bytes allocated
	*static_cast<size*>(allocation) = _bytes;

	// Assign the return value of the allocation
	_allocation = reinterpret_cast<_Type*>((size)allocation + sizeof(size));

	return Status::SUCCESS;

SLR_EXCEPTION(COULD_NOT_ALLOCATE):
	SLR_NO_OPERATION; // Fall through
SLR_EXCEPTION(INVALID_SIZE):
	return Status::FAIL;
}

/**
* Frees memory allocated with MemAlloc(...)
* Once the memory is freed, _allocation is set to nullptr
* If _allocation is a nullptr, a warning is logged
*/
template<typename _Type = void>
Status MemFree(SLR_RETURN(_Type*) _allocation)
{
	SLR_ASSERT_WARNING(_allocation != nullptr, NULLPTR_PROVIDED, "Attempted to free a nullptr");

	// Get a pointer to where the actual allocation was
	void* allocationPointer = reinterpret_cast<void*>((size)_allocation - sizeof(size));

	// Free the allocation
	std::free(allocationPointer);

	// Assign the pointer of the allocation to nullptr
	_allocation = nullptr;

	return Status::SUCCESS;

SLR_EXCEPTION(NULLPTR_PROVIDED):
	return Status::FAIL;
}

/**
* Returns the amount of bytes allocated by MemAlloc(...) for a given allocation
* Providing a pointer which was not allocated by MemAlloc(...) results in undefined behavior
*/
template<typename _Type = void>
Status MemSize(const _Type* _allocation, SLR_RETURN(size) _bytes)
{
	// Get a non-const, void pointer to _allocation
	void* allocationPointer = const_cast<void*>(static_cast<const void*>(_allocation));

	// Get a pointer to the allocation size
	const size* allocationSize = reinterpret_cast<size*>((size)allocationPointer - sizeof(size));

	// Set the return value for the allocation size
	_bytes = *allocationSize;

	return Status::SUCCESS;
}

SLR_NAMEPSACE_END

#endif // ifndef SLR_MEMORY_ALLOCATION