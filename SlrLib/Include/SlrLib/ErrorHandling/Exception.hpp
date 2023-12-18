#pragma once
#ifndef SLR_ERRORHANDLING_EXCEPTION
#define SLR_ERRORHANDLING_EXCEPTION

#include "SlrLib/Internal/Namespace.hpp"
#include "SlrLib/Utilities/Types.hpp"

SLR_NAMESPACE_BEGIN

/**
* An enum which is the return type for all function calls to specify whether a call succeeded or not
*/
enum class Status : word
{
	// Ensure SUCCESS can always be casted to true
	SUCCESS = static_cast<word>(true),

	// Ensure FAIL can always be casted to false
	FAIL = static_cast<word>(false)
};

/**
* Ensure SUCCESS and FAIL have not been evaluated into the same value
*/
static_assert(
	Status::SUCCESS != Status::FAIL,
	"Status::SUCCESS and Status::FAIL evaluated to be the same value"
);

SLR_NAMEPSACE_END

#endif // ifndef SLR_ERRORHANDLING_EXCEPTION