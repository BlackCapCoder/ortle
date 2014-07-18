#ifndef ORTLE_OPENGL_VERTEX_ARRAY_HPP
#define ORTLE_OPENGL_VERTEX_ARRAY_HPP


#include "core330.hpp"




namespace OpenGL {


class VertexArray {

public:

	VertexArray();
	explicit VertexArray(GLuint handle);

	VertexArray(VertexArray&& other);
	VertexArray& operator=(VertexArray&& other);

	~VertexArray();

	friend void swap(VertexArray& first, VertexArray& second);


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

