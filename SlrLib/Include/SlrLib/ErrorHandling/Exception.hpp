/**
* The functions contained within this file are an exemption to the typical method of returning Status. This is because if
* any of the function calls fail and the caller asserts the result, this could cause infinite recursive calls to the
* function. Therefore, all of these functions must be designed in a way which cannot throw an error, or require an error
* message to be logged.
*/

#pragma once
#ifndef SLR_ERRORHANDLING_EXCEPTION
#define SLR_ERRORHANDLING_EXCEPTION

#include <type_traits>

#include "SlrLib/ErrorHandling/Logger.hpp"
#include "SlrLib/Internal/Namespace.hpp"
#include "SlrLib/Utilities/Types.hpp"

SLR_NAMESPACE_BEGIN

/**
* An enum which is the return type for all function calls to specify whether a call succeeded or not
* All functions should return this, and the value which would otherwise be returned should be passed by reference then the
* function will modify the value of it; this should be referred to as the return value(s), despite Status being the return
* value in the traditional sense. If the function call results in Status::FAIL, the return value(s) should be assumed to be
* in an undefined state. This is a replacement for the built-in `throw` mechanism, but functions should not have the
* specifier `noexcept` applied to them - Any accidental throws should be caught at the highest level of the call stack to
* facilitate debugging.
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

/**
* This class is purely to hide the implementation from the user to make it clear not to interact with anything contained
* within
*/
class ExceptionImplementation
{
public:
	template<typename>
	friend void SetLogger();

	/**
	* Returns a pointer to the logger
	*/
	static inline constexpr Logger* GetLogger()
	{
		return ExceptionImplementation::logger;
	}

private:
	/**
	* A pointer to the logger object
	*/
	static inline Logger* logger = new DefaultLogger();
};

/**
* Sets the logger to be used when logging messages
* The logger type must inherit from Logger
*/
template<typename _Logger>
void SetLogger()
{
	static_assert(std::is_base_of<Logger, _Logger>::value, "Logger type must inherit from Logger");

	// Delete the current logger
	delete ExceptionImplementation::logger;

	// Create new logger with specified type
	ExceptionImplementation::logger = new _Logger();
}

/**
* If _condition evaluates to false, _message will be logged as an error and the body of the macro will be executed
* Example usage:
*     SLR_ASSERT_ERROR(myCondition, "My condition was not met")
*     {
*         return Status::FAIL;
*     }
*     // Regular flow control with no exceptions
* The majority of the time, this should just return Status::FAIL, though there may be some circumstances where the exception
* would require some clean-up of resources, in which case, that would be handled within the body of the macro, then also
* likely return FAIL
*/
#define SLR_ASSERT_ERROR(_condition, _message) \
	if (static_cast<bool>(_condition) == false) { \
		ExceptionImplementation::GetLogger()->operator()(_message, LogLevel::ERROR); \
	} \
	if (static_cast<bool>(_condition) == false)

/**
* If _condition evaluates to false, _message is logged as a warning and the body of the macro will be executed
* Refer to SLR_ASSERT_ERROR(...) for full explanation of this mechanism
*/
#define SLR_ASSERT_WARNING(_condition, _message) \
	if (static_cast<bool>(_condition) == false) { \
		ExceptionImplementation::GetLogger()->operator()(_message, LogLevel::WARNING); \
	} \
	if (static_cast<bool>(_condition) == false)

/**
* If _condition evaluates to false, _message is logged as info and the body of the macro will be executed
* Refer to SLR_ASSERT_ERROR(...) for full explanation of this mechanism
*/
#define SLR_ASSERT_INFO(_condition, _message) \
	if (static_cast<bool>(_condition) == false) { \
		ExceptionImplementation::GetLogger()->operator()(_message, LogLevel::INFO); \
	} \
	if (static_cast<bool>(_condition) == false)

/**
* Logs an error if _condition evaluates to false
*/
#define SLR_ERROR(_condition, _message) \
	do { \
		if (static_cast<bool>(_condition) == false) { \
			ExceptionImplementation::GetLogger()->operator()(_message, LogLevel::ERROR); \
		} \
	} while (0)

/**
* Logs a warning if _condition evaluates to false
*/
#define SLR_WARNING(_condition, _message) \
	do { \
		if (static_cast<bool>(_condition) == false) { \
			ExceptionImplementation::GetLogger()->operator()(_message, LogLevel::WARNING); \
		} \
	} while (0)

/**
* Logs info if _condition evaluates to false
*/
#define SLR_INFO(_condition, _message) \
	do { \
		if (static_cast<bool>(_condition) == false) { \
			ExceptionImplementation::GetLogger()->operator()(_message, LogLevel::INFO); \
		} \
	} while (0)

SLR_NAMESPACE_END

#endif // ifndef SLR_ERRORHANDLING_EXCEPTION