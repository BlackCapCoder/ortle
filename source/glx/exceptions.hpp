#ifndef ORTLE_GLX_EXCEPTIONS_HPP
#define ORTLE_GLX_EXCEPTIONS_HPP


#include <stdexcept>




namespace GLX {


class InitializationError : public std::runtime_error {

public:

	InitializationError(char const* message)
		: std::runtime_error(message)
	{}

};


} // namespace GLX


#endif

