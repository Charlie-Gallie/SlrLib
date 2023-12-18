#pragma once
#ifndef SLR_MEMORY_SHAREDPOINTER
#define SLR_MEMORY_SHAREDPOINTER

#include <type_traits>

#include "SlrLib/Internal/Namespace.hpp"
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

public:
	/**
	* Constructor
	* Assigns referenceCount and value to be nullptr to signify this shared owner instance does not hold and reference to an
	* object
	*/
	Shared() noexcept
	{
		// Signify we are not holding a reference to an object
		referenceCount = nullptr;
		value = nullptr;
	}

	/**
	* Copy constructor
	* Add a new reference to the object being held to by _other
	*/
	Shared(const Shared& _other) noexcept
	{
		// Copy the pointer to the reference count
		this->referenceCount = _other.referenceCount;

		// Copy the pointer to the object
		this->value = _other.value;

		// Increment the amount of references there are to the object
		*this->referenceCount += 1;
	}

	/**
	* Move constructor
	* Take ownership of the object being held by _other
	* We do not increment nor decrement the reference count because we are purely taking ownership
	*/
	Shared(Shared&& _other) noexcept
	{
		// Copy the pointer to the reference count
		this->referenceCount = _other.referenceCount;

		// Copy the pointer to the object
		this->value = _other.value;

		// Set the members of _other to nullptr so it doesn't potentially delete the once it's being destructed
		_other.referenceCount = nullptr;
		_other.value = nullptr;
	}

	/**
	* Destructor
	* If this is holding a reference to an object, it will decrement the reference count and delete the object if necessary
	*/
	~Shared() noexcept
	{
		// Check if this instance is holding a reference to an object
		if ((this->referenceCount == nullptr) || (this->value == nullptr))
		{
			// If not, exit the destructor
			return;
		}

		// Decrement the reference count
		*this->referenceCount -= 1;

		// Check if there are no references left
		if (*this->referenceCount == 0)
		{
			// Destruct the object
			value->~_Type();

			// TODO: Free object
		}
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
};

SLR_NAMEPSACE_END

#endif // ifndef SLR_MEMORY_SHAREDPOINTER