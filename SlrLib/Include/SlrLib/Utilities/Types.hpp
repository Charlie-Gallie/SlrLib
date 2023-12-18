#pragma once
#ifndef SLR_UTILITIES_TYPES
#define SLR_UTILITIES_TYPES

#include <cstdint>

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
*/
using word = WordSize<sizeof(void*)>::PointerType;

SLR_NAMEPSACE_END

#endif // ifndef SLR_UTILITIES_TYPES