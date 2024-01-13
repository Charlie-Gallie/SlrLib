#pragma once
#ifndef SLR_ERRORHANDLING_LOGGER
#define SLR_ERRORHANDLING_LOGGER

#include <source_location>
#include <string_view>

#include "SlrLib/Utilities/Types.hpp"
#include "SlrLib/Internal/Namespace.hpp"

SLR_NAMESPACE_BEGIN

// Undefines ERROR if it's already defined and later redefines it to its initial value
// This is done because we want to have the name LogLevel::ERROR, but if ERROR is already defined, the preprocessor messes
// it up - The Windows API defines ERROR, therefore this issue will likely arise
#ifdef ERROR
#define SLR_INITIAL_ERROR_VALUE ERROR
#undef ERROR
#endif

/**
* The log level of a logged message
* The different level will format the message differently with more or less info, based on the loggers discretion
*/
enum class LogLevel : word
{
	ERROR,
	WARNING,
	INFO
};

// Redefines ERROR to its initial value, if it was originally defined
#ifdef SLR_INITIAL_ERROR_VALUE
#define ERROR SLR_INITIAL_ERROR_VALUE
#endif

/**
* The base class for any logger object
*/
class Logger
{
public:
	/**
	* Default constructor
	*/
	Logger() = default;

	/**
	* This operator is what actually logs a message
	*/
	virtual Logger& operator()(
		const std::string_view,
		const LogLevel,
		const std::source_location& = std::source_location::current()
	) = 0;

	/**
	* Virtual destructor
	*/
	virtual ~Logger() = default;
};

/**
* The default logger
*/
class DefaultLogger : public Logger
{
public:
	/**
	* Default constructor
	*/
	DefaultLogger() = default;

	/**
	* Override for the () operator to log a message
	*/
	DefaultLogger& operator()(
		const std::string_view,
		const LogLevel,
		const std::source_location&
	) override;
};

SLR_NAMESPACE_END

#endif // ifndef SLR_ERRORHANDLING_LOGGER