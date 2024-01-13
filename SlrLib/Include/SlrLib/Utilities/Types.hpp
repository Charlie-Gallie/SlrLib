#pragma once
#ifndef SLR_UTILITIES_TYPES
#define SLR_UTILITIES_TYPES

#include <cstdint>
#include <type_traits>

#include "SlrLib/Internal/Namespace.hpp"

SLR_NAMESPACE_BEGIN

using i8 = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using i64 = std::int64_t;

using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;

using char8 = char;
using char16 = char16_t;
using char32 = char32_t;
using uchar8 = unsigned char;

using byte = unsigned char;

using size = decltype(sizeof(0));

/**
* A struct to have template specialization applied
* The specialization will take the size of a pointer to determine the platform word size
*/
template<size _PointerSize>
struct WordSize;

/**
* Specialization for x86
*/
template<>
struct WordSize<4>
{
	using PointerType = u32;
};

/**
* Specialization for x64
*/
template<>
struct WordSize<8>
{
	using PointerType = u64;
};

/**
* Get the word size of the platform
* It is possible for this to be the wrong value, but should be correct when compiling for the majority of platforms
* Should look for a fully reliable way to get the word size of the platform
*/
using word = WordSize<sizeof(void*)>::PointerType;

/**
* Takes _Type and forms ReturnType::Type into _Type as the datatype which should be returned from a function call
*/
template<typename _Type>
class ReturnType
{
	static_assert(!std::is_const<_Type>::value, "Return type may not be const");
	static_assert(!std::is_reference<_Type>::value, "Return type may not be initially declared as a reference");

public:
	using Type = _Type&;
};

/**
* Converts _type into the return type for functions
* This is to make the return values within function parameters explicit to differentiate it from taking a regular reference
* Example usage:
*     Status Multiply(SLR_RETURN(i32) _c, i32 _a, i32 _b) {
*         _c = _a * _b;
*         // ...
*     }
* For consistency, try to have the return value(s) as the first parameters
*/
#define SLR_RETURN(_type) typename ReturnType<_type>::Type

SLR_NAMESPACE_END

#endif // ifndef SLR_UTILITIES_TYPES