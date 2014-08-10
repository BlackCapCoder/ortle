#ifndef ORTLE_X11_EXCEPTIONS_HPP
#define ORTLE_X11_EXCEPTIONS_HPP


#include <stdexcept>




namespace X11 {


class InitializationError : public std::runtime_error {

public:

	InitializationError(char const* message)
		: std::runtime_error(message)
	{}

};




class IncompatibleVersion : public std::runtime_error {

public:

	IncompatibleVersion(char const* message)
		: std::runtime_error(message)
	{}

};




class MissingExtension : public std::runtime_error {

public:

	MissingExtension(char const* message)
		: std::runtime_error(message)
	{}

};


} // namespace X11


#endif

