#pragma once
#ifndef SLR_MATH_FUNCTIONS
#define SLR_MATH_FUNCTIONS

#include <type_traits>
#include <cmath>
#include <utility>

#include "SlrLib/ErrorHandling/Exception.hpp"
#include "SlrLib/Internal/Namespace.hpp"
#include "SlrLib/Utilities/Types.hpp"

SLR_NAMESPACE_BEGIN

/**
* Returns the square root of a value
*/
template<typename _Type>
Status Sqrt(SLR_RETURN(_Type) _result, const _Type _value)
{
	// TODO: Error handling

	_result = std::sqrt(_value);

	return Status::SUCCESS;
}

/**
* 
*/
template<typename _Type, typename ... _Others>
Status MinHelper(SLR_RETURN(_Type) _result, const _Type& _first, _Others&&... _others);

/**
* 
*/
template<typename _Type>
Status Min(SLR_RETURN(_Type) _result, const _Type _value)
{
	_result = _value;
	return Status::SUCCESS;
}

/**
* 
*/
template<typename _Type>
Status Min(SLR_RETURN(_Type) _result, const _Type _left, const _Type _right)
{
	_result = _left < _right ? _left : _right;

	return Status::SUCCESS;
}

/**
* 
*/
template<typename _Type, typename ... _Others>
Status Min(SLR_RETURN(_Type) _result, const _Type _first, _Others&&... _others)
{
	return MinHelper(_result, _first, std::forward<_Others>(_others)...);
}

/**
* 
*/
template<typename _Type, typename ... _Others>
Status MinHelper(SLR_RETURN(_Type) _result, const _Type& _first, _Others&&... _others)
{
	_Type temp;
	Min(temp, std::forward<_Others>(_others)...);
	Min(_result, _first, temp);

	return Status::SUCCESS;
}

SLR_NAMESPACE_END

#endif // ifndef SLR_MATH_FUNCTIONS