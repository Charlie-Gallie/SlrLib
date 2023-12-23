#pragma once
#ifndef SLR_MEMORY_SHAREDPOINTER
#define SLR_MEMORY_SHAREDPOINTER

#include <type_traits>
#include <utility>

#include "SlrLib/ErrorHandling/Exception.hpp"
#include "SlrLib/Internal/Namespace.hpp"
#include "SlrLib/Memory/Allocation.hpp"
#include "SlrLib/Utilities/Types.hpp"

SLR_NAMESPACE_BEGIN

/**
* 
*/
template<typename _Type>
class Shared
{
	static_assert(!std::is_pointer<_Type>::value, "Shared pointer type cannot be a pointer");
	static_assert(!std::is_array<_Type>::value, "Shared pointer type cannot be an array");

	template<typename _Type, typename ... _Arguments>
	friend Status CreateShared(Shared<_Type>&, _Arguments&& ...);

public:
	/**
	* Constructor
	* Assigns referenceCount and value to be nullptr to signify this shared owner instance does not hold and reference to an
	* object
	*/
	Shared()
	{
		// Signify we are not holding a reference to an object
		referenceCount = nullptr;
		value = nullptr;
	}

	/**
	* Copy constructor
	* Add a new reference to the object being held to by _other
	*/
	Shared(const Shared& _other)
	{
		// Make this instance a new reference to _other
		NewReference(_other);
	}

	/**
	* Move constructor
	* Take ownership of the object being held by _other
	*/
	Shared(Shared&& _other)
	{
		// Take the reference from _other and assign it to this instance
		TakeReference(std::move(_other));
	}

	/**
	* Destructor
	* If this is holding a reference to an object, it will decrement the reference count and delete the object if necessary
	*/
	~Shared()
	{
		// Check if this instance is holding a reference to an object
		bool isHoldingReference;
		IsHoldingReference(isHoldingReference);

		if (!isHoldingReference)
		{
			// If not, exit the destructor
			return;
		}

		// Decrement the reference count
		*this->referenceCount -= 1;

		// Check if there are no references left
		if (*this->referenceCount == 0)
		{
			// Call the destructor for the object
			// This must be a manual action because we are not using new and delete
			value->~_Type();

			// Delete the object
			// The reference count is allocated directly after the object as a single allocation, therefore freeing object
			// will also free referenceCount
			Status status = MemFree<_Type>(value);
			SLR_ERROR(status == Status::SUCCESS, "Could not free object held by shared pointer");
		}
	}

	/**
	* Returns a boolean stating if this instance is holding a reference to an object, where true means it is holding a
	* reference, and false means it is not
	*/
	inline Status IsHoldingReference(bool& _isHoldingReference) const
	{
		_isHoldingReference = ((value != nullptr) && (referenceCount != nullptr));

		return Status::SUCCESS;
	}

	/**
	* Returns the number of references to the object
	* An error is logged if this function is called while not holding a reference to any object
	* If IsHoldingReference(...) returns true immediately prior to calling this function, it will not throw an exception and
	* the reference count will be >=1
	*/
	inline Status GetReferenceCount(size& _references) const
	{
		// Check if we're holding a reference to an object
		bool isHoldingReference;
		IsHoldingReference(isHoldingReference);
		SLR_ASSERT_ERROR(isHoldingReference == true, NOT_HOLDING_REFERENCE, "Not holding a reference to any object");

		_references = *this->referenceCount;

		return Status::SUCCESS;

	SLR_EXCEPTION(NOT_HOLDING_REFERENCE):
		return Status::FAIL;
	}

	/**
	* Returns a reference to the object stored
	*/
	_Type& operator*()
	{
		return *value;
	}

	/**
	* Returns a reference to the object stored
	*/
	_Type& operator->()
	{
		return *value;
	}

	/**
	* Returns a const reference to the object stored
	*/
	const _Type& operator*() const
	{
		return *value;
	}

	/**
	* Returns a const reference to the object stored
	*/
	const _Type& operator->() const
	{
		return *value;
	}

private:
	/**
	* A pointer to the reference counts which exist for the object
	* If this is nullptr then this instance of a shared pointer does not hold a reference to an object
	* This will always point to value + sizeof(_Type) because the reference count is allocated proceeding the object
	*/
	size* referenceCount = nullptr;

	/**
	* A pointer to the object being contained by this shared pointer
	*/
	_Type* value = nullptr;

	/**
	* Takes a reference from _other and assign it ourself
	* We do not increment nor decrement the reference count because we are purely taking ownership
	*/
	inline Status TakeReference(Shared&& _other)
	{
		// Copy the pointer to the reference count
		this->referenceCount = _other.referenceCount;

		// Copy the pointer to the object
		this->value = _other.value;

		// Set the members of _other to nullptr so it doesn't potentially delete the once it's being destructed
		_other.referenceCount = nullptr;
		_other.value = nullptr;

		return Status::SUCCESS;
	}

	/**
	* Create a new reference to the object pointed to by _other.value and increment the reference count
	* _other may not be holding a reference to any object, so we must check for that
	*/
	inline Status NewReference(const Shared& _other)
	{
		// Copy the pointer to the reference count
		this->referenceCount = _other.referenceCount;

		// Copy the pointer to the object
		this->value = _other.value;

		// Check if this is holding a valid reference
		bool isHoldingReference;
		IsHoldingReference(isHoldingReference);

		if (isHoldingReference)
		{
			// Increment the amount of references there are to the object
			*this->referenceCount += 1;
		}

		return Status::SUCCESS;
	}
};

/**
* 
*/
template<typename _Type, typename ... _Arguments>
Status CreateShared(Shared<_Type>& _shared, _Arguments&& ... _arguments)
{
	// Get the type of the reference count
	using ReferenceCountType = decltype(Shared<_Type>::referenceCount);

	// Allocate memory for the shared pointer object
	// We allocate the object and reference count together so we don't have to do multiple allocations, and so getting the
	// value of the reference count can be optimized
	void* allocation = nullptr;
	Status status = MemAlloc(sizeof(_Type) + sizeof(ReferenceCountType), allocation);
	SLR_ASSERT_ERROR(status == Status::SUCCESS, COULD_NOT_ALLOCATE, "Could not allocate memory for shared pointer object");

	// [ value ][ reference count ]
	// ^
	// allocation
	_shared.value = reinterpret_cast<_Type*>(allocation);

	// [ value ][ reference count ]
	//          ^
	//          allocation + sizeof(_Type)
	_shared.referenceCount = reinterpret_cast<ReferenceCountType>((size)allocation + sizeof(_Type));

	// This is the first reference
	*_shared.referenceCount = 1;

	// Construct the object in-place of where _shared.value points to
	new(_shared.value) _Type(std::forward<_Arguments>(_arguments)...);

	return Status::SUCCESS;

SLR_EXCEPTION(COULD_NOT_ALLOCATE):
	return Status::FAIL;
}

SLR_NAMEPSACE_END

#endif // ifndef SLR_MEMORY_SHAREDPOINTER