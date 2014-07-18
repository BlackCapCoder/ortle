#ifndef ORTLE_OPENGL_SHADER_HPP
#define ORTLE_OPENGL_SHADER_HPP


#include "core330.hpp"




namespace OpenGL {


class Shader {

public:

	explicit Shader(GLuint handle);
	Shader(GLenum type, char const* source);

	Shader(Shader&& other);
	Shader& operator=(Shader&& other);

	~Shader();

	friend void swap(Shader& first, Shader& second);


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

