#ifndef ORTLE_EXCEPTIONS_HPP
#define ORTLE_EXCEPTIONS_HPP


#include <stdexcept>




class FramebufferError : public std::runtime_error {

public:

	FramebufferError(char const* message)
		: std::runtime_error(message)
	{}

};




class InitializationError : public std::runtime_error {

public:

	InitializationError(char const* message)
		: std::runtime_error(message)
	{}

};


#endif

