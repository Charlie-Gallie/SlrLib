#include "SlrLib/ErrorHandling/Logger.hpp"

#include <iostream>

SLR_NAMESPACE_BEGIN

DefaultLogger& DefaultLogger::operator()(const std::string_view _message, const LogLevel _level, const std::source_location& _sourceLocation)
{
	/**
	* This is a temporary implementation
	*/

	const char* prefix = nullptr;

	switch (_level)
	{
	case LogLevel::ERROR:
		prefix = "[Error]";
		break;

	case LogLevel::WARNING:
		prefix = "[Warning]";
		break;

	case LogLevel::INFO:
		prefix = "[Info]";
		break;

	default:
		prefix = "[Unknown]";
		break;
	}

	std::cout << prefix << ": " << _message << std::endl;

	return *this;
}

SLR_NAMESPACE_END