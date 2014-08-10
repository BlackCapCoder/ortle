#ifndef ORTLE_OPENGL_PROGRAM_HPP
#define ORTLE_OPENGL_PROGRAM_HPP


#include "core330.hpp"
#include "shader.hpp"

#include <initializer_list>




namespace OpenGL {


class Program {

public:

	explicit Program(GLuint handle);
	explicit Program(std::initializer_list<Shader*> shaders);

	Program(Program&& other);
	Program& operator=(Program&& other);

	~Program();

	friend void swap(Program& first, Program& second);


public:

	operator GLuint() const
	{
		return m_handle;
	}


private:

	GLuint m_handle;

};


} // namespace OpenGL


#endif

