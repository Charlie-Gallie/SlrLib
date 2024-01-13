#pragma once
#ifndef SLR_MATH_VECTOR2
#define SLR_MATH_VECTOR2

#include <type_traits>

#include "SlrLib/Internal/Namespace.hpp"
#include "SlrLib/Math/Functions.hpp"
#include "SlrLib/Utilities/Types.hpp"

SLR_NAMESPACE_BEGIN

/**
* 
*/
template<typename _Type>
struct Vector2
{
	static_assert(std::is_arithmetic<_Type>::value, "Type of vector template must be numeric");

	/**
	* The x component of the vector
	*/
	_Type x;

	/**
	* The y component of the vector
	*/
	_Type y;

	/**
	* Default constructor
	* Default initializes the x and y components of the vector to 0
	*/
	constexpr Vector2() : x(0), y(0) {}

	/**
	* Constructor
	* Takes one value which is assigned to both x and y components
	*/
	constexpr Vector2(const _Type& _value) : x(_value), y(_value) {}

	/**
	* Constructor
	* Takes two values to respectively assign to the x and y components
	*/
	constexpr Vector2(const _Type& _x, const _Type& _y) : x(_x), y(_y) {}

	/**
	* Constructor
	* Takes an instance of a vector and copies its data
	*/
	constexpr Vector2(const Vector2& _other) : x(_other.x), y(_other.y) {}

	/**
	* Assignment operator
	* Takes a value and assigns it to both x and y components
	*/
	constexpr Vector2& operator=(const _Type& _value) { x = _value; y = _value; return *this; }

	/**
	* Assignment operator
	* Takes an instance of a vector and copies its data
	*/
	constexpr Vector2& operator=(const Vector2& _rhs) { x = _rhs.x; y = _rhs.y; return *this; }

	/**
	* Equal to operator
	* Returns true if the given vector is equal to this instance
	* Both the x and y components are checked for inequality
	*/
	constexpr bool operator==(const Vector2& _rhs) const { return _rhs.x == x && _rhs.y == y; }

	/**
	* Not equal to operator
	* Returns true is the given vector is not equal to this instance
	* Both the x and y components are checked for inequality
	*/
	constexpr bool operator!=(const Vector2& _rhs) const { return _rhs.x != x || _rhs.y != y; }

	/**
	* Unary plus operator
	*/
	constexpr Vector2 operator+() const { return Vector2(+x, +y); }

	/**
	* Unary negation operator
	* Returns a vector where the x and y components are negated
	*/
	constexpr Vector2 operator-() const { return Vector2(-x, -y); }

	/**
	* Pre-increment operator
	* Increments both x and y components of the vector and returns the result as a reference
	*/
	constexpr Vector2& operator++() { ++x; ++y; return *this; }

	/**
	* Pre-decrement operator
	* Decrements both x and y components of the vector and returns the result as a reference
	*/
	constexpr Vector2& operator--() { --x; --y; return *this; }

	/**
	* Post-increment operator
	* Increments both x and y components of the vector but returns the result prior to incrementation
	*/
	constexpr Vector2 operator++(int) { Vector2 temp = *this; ++x; ++y; return temp; }

	/**
	* Post-decrement operator
	* Decrements both x and y components of the vector but returns the result prior to decrementation
	*/
	constexpr Vector2 operator--(int) { Vector2 temp = *this; --x; --y; return temp; }

	/**
	*
	*/
	constexpr Vector2 operator+(const Vector2& _rhs) const { return Vector2(x + _rhs.x, y + _rhs.y); }

	/**
	*
	*/
	constexpr Vector2 operator-(const Vector2& _rhs) const { return Vector2(x - _rhs.x, y - _rhs.y); }

	/**
	*
	*/
	constexpr Vector2 operator*(const Vector2& _rhs) const { return Vector2(x * _rhs.x, y * _rhs.y); }

	/**
	*
	*/
	constexpr Vector2 operator/(const Vector2& _rhs) const { return Vector2(x / _rhs.x, y / _rhs.y); }

	/**
	*
	*/
	constexpr Vector2 operator+(const _Type& _rhs) const { return Vector2(x + _rhs, y + _rhs); }

	/**
	*
	*/
	constexpr Vector2 operator-(const _Type& _rhs) const { return Vector2(x - _rhs, y - _rhs); }

	/**
	*
	*/
	constexpr Vector2 operator*(const _Type& _rhs) const { return Vector2(x * _rhs, y * _rhs); }

	/**
	*
	*/
	constexpr Vector2 operator/(const _Type& _rhs) const { return Vector2(x / _rhs, y / _rhs); }

	/**
	*
	*/
	constexpr Vector2& operator+=(const Vector2& _rhs) { x += _rhs.x; y += _rhs.y; return *this; }

	/**
	*
	*/
	constexpr Vector2& operator-=(const Vector2& _rhs) { x -= _rhs.x; y -= _rhs.y; return *this; }

	/**
	*
	*/
	constexpr Vector2& operator*=(const Vector2& _rhs) { x *= _rhs.x; y *= _rhs.y; return *this; }

	/**
	*
	*/
	constexpr Vector2& operator/=(const Vector2& _rhs) { x /= _rhs.x; y /= _rhs.y; return *this; }

	/**
	*
	*/
	constexpr Vector2& operator+=(const _Type& _rhs) { x += _rhs; y += _rhs; return *this; }

	/**
	*
	*/
	constexpr Vector2& operator-=(const _Type& _rhs) { x -= _rhs; y -= _rhs; return *this; }

	/**
	*
	*/
	constexpr Vector2& operator*=(const _Type& _rhs) { x *= _rhs; y *= _rhs; return *this; }

	/**
	*
	*/
	constexpr Vector2& operator/=(const _Type& _rhs) { x /= _rhs; y /= _rhs; return *this; }

	/**
	* Returns the dot product of two vectors
	*/
	constexpr Status Dot(SLR_RETURN(_Type) _result, const Vector2& _other) const
	{
		// Calculate the dot product of the two vectors
		_result = (this->x * _other.x) + (this->y * _other.y);

		return Status::SUCCESS;
	}

	/**
	* Returns the cross product of two vectors
	*/
	constexpr Status Cross(SLR_RETURN(_Type) _result, const Vector2& _other) const
	{
		// Calculate the cross product of the two vectors
		_result = (this->x * _other.y) - (this->y * _other.x);

		return Status::SUCCESS;
	}

	/**
	* Returns the magnitude of the vector
	*/
	constexpr Status Magnitude(SLR_RETURN(_Type) _result) const
	{
		// Calculate the magnitude squared
		const _Type magnitudeSquared = (this->x * this->x) + (this->y * this->y);

		// Assign the return value for the square root of the magnitude
		Status sqrtStatus = Sqrt(_result, magnitudeSquared);

		SLR_ASSERT_ERROR(sqrtStatus == Status::SUCCESS, "Could not calculate square root of magnitude squared")
		{
			return Status::FAIL;
		}

		return Status::SUCCESS;
	}

	/**
	* Returns the magnitude squared of the vector
	* This is an option for efficiency so that the user doesn't take the magnitude (which has the square root applied) then
	* square it
	*/
	constexpr Status MagnitudeSquared(SLR_RETURN(_Type) _result) const
	{
		// Calculate the magnitude squared
		_result = (this->x * this->x) + (this->y * this->y);

		return Status::SUCCESS;
	}

	/**
	* Returns the normalized version of the vector
	*/
	constexpr Status Normalized(SLR_RETURN(Vector2) _result) const
	{
		// Get the magnitude of the vector
		_Type magnitude;
		Status magnitudeStatus = this->Magnitude(magnitude);

		SLR_ASSERT_ERROR(magnitudeStatus == Status::SUCCESS, "Could not calculate the magnitude to normalize vector")
		{
			return Status::FAIL;
		}

		SLR_ASSERT_ERROR(magnitude != 0, "Magnitude resulted to zero and therefore cannot be normalized")
		{
			return Status::FAIL;
		}

		// Get the inverse magnitude of the vector
		const _Type inverseMagnitude = _Type(1) / magnitude;

		// Normalize the vector and return it
		_result = Vector2(this->x * inverseMagnitude, this->y * inverseMagnitude);

		return Status::SUCCESS;
	}

	/**
	* Returns a vector of a different data type
	*/
	template<typename _ParseType>
	constexpr Status AsType(SLR_RETURN(Vector2<_ParseType>) _result) const
	{
		// Convert the x and y components to the new type
		_result = Vector2<_ParseType>(
			static_cast<_ParseType>(this->x),
			static_cast<_ParseType>(this->y)
		);

		return Status::SUCCESS;
	}

	/**
	* Returns a unit vector
	* The return value is (1, 1) normalized
	*/
	static constexpr Status GetUnitVector(SLR_RETURN(Vector2) _result)
	{
		// Get a unit vector
		Status normalizedStatus = Vector2(1, 1).Normalized(_result);
		SLR_ASSERT_ERROR(normalizedStatus == Status::SUCCESS, "Could not normaize vector")
		{
			return Status::FAIL;
		}

		return Status::SUCCESS;
	}

	/**
	* Returns a zero vector
	* The x and y components are assigned to 0
	*/
	static constexpr Status GetZeroVector(SLR_RETURN(Vector2) _result)
	{
		_result = Vector2(0, 0);

		return Status::SUCCESS;
	}
};

using Vector2u32 = Vector2<u32>;
using Vector2i32 = Vector2<i32>;

SLR_NAMESPACE_END

#endif // ifndef SLR_MATH_VECTOR2