#ifndef ORTLE_OPENGL_BUFFER_HPP
#define ORTLE_OPENGL_BUFFER_HPP


#include "core330.hpp"




namespace OpenGL {


class Buffer {

public:

	Buffer();
	explicit Buffer(GLuint handle);

	Buffer(Buffer&& other);
	Buffer& operator=(Buffer&& other);

	~Buffer();

	friend void swap(Buffer& first, Buffer& second);


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

