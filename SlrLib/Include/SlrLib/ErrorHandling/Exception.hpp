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
* If _condition evaluates to false, it will jump to _label and _message is logged
* Use SLR_EXCEPTION as the label to jump to
* A label name must be referred to exactly once in any assertion per scope, meaning, two assertions cannot use the same
* exception
* Example usage:
*     SLR_ASSERT_ERROR(condition, CONDITION_WAS_NOT_MET, "Some condition was not met");
*     // Code to be executed for regular execution
* SLR_EXCEPTION(CONDITION_WAS_NOT_MET):
*     // Code to recover from assertion (usually return Status::FAIL)
*/
#define SLR_ASSERT_ERROR(_condition, _label, _message) \
	if (static_cast<bool>(_condition) == false) { \
		ExceptionImplementation::GetLogger()->operator()(_message, LogLevel::ERROR); \
		goto _label; \
	} \
	/* The opening brace matches with the closing brace where the exception is defined with SLR_EXCEPTION */ \
	/* The braces exist to make it possible to declare variables below an assertion, otherwise the goto breaks it */ \
	/* The cast to void makes it so the user is required to place a semicolon after the assertion */ \
	{ (void)0

/**
* If _condition evaluates to false, _message is logged as a warning and flow will jump to _label
* Refer to SLR_ASSERT_ERROR for full explanation of this mechanism
*/
#define SLR_ASSERT_WARNING(_condition, _label, _message) \
	if (static_cast<bool>(_condition) == false) { \
		ExceptionImplementation::GetLogger()->operator()(_message, LogLevel::WARNING); \
		goto _label; \
	} \
	{ (void)0

/**
* If _condition evaluates to false, _message is logged as info and flow will jump to _label
* Refer to SLR_ASSERT_ERROR for full explanation of this mechanism
*/
#define SLR_ASSERT_INFO(_condition, _label, _message) \
	if (static_cast<bool>(_condition) == false) { \
		ExceptionImplementation::GetLogger()->operator()(_message, LogLevel::INFO); \
		goto _label; \
	} \
	{ (void)0

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

/**
* The jump location of an assertion
* Handle exceptions in reverse order to the order they were defined, e.g.:
*     SLR_ASSERT_ERROR(..., EX_1, ...);
*     SLR_ASSERT_ERROR(..., EX_2, ...);
*     // ...
* SLR_EXCEPTION(EX_2):
*     // ...
* SLR_EXCEPTION(EX_1):
*     // ...
*/
#define SLR_EXCEPTION(_label) } goto _label; _label

SLR_NAMEPSACE_END

#endif // ifndef SLR_ERRORHANDLING_EXCEPTION