#ifndef ORTLE_OPENGL_EXCEPTIONS_HPP
#define ORTLE_OPENGL_EXCEPTIONS_HPP


#include "core330.hpp"

#include <stdexcept>




namespace OpenGL {


class StateError : public std::runtime_error {

public:

	StateError(char const* message, GLenum code)
		: std::runtime_error(message)
		, m_code(code)
	{}

	GLenum code() const
	{
		return m_code;
	}


private:

	GLenum m_code;
	
};




class ShaderError : public std::runtime_error {

public:

	ShaderError(char const* message)
		: std::runtime_error(message)
	{}

};




class ProgramError : public std::runtime_error {

public:

	ProgramError(char const* message)
		: std::runtime_error(message)
	{}

};


} // namespace OpenGL


#endif

