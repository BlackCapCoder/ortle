#ifndef ORTLE_OPENGL_TEXTURE_HPP
#define ORTLE_OPENGL_TEXTURE_HPP


#include "core330.hpp"




namespace OpenGL {


class Texture {

public:

	Texture();
	explicit Texture(GLuint handle);

	Texture(Texture&& other);
	Texture& operator=(Texture&& other);

	~Texture();

	friend void swap(Texture& first, Texture& second);


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

