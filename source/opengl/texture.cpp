#include "texture.hpp"

#include "core330.hpp"

#include <cassert>

#include <utility>




namespace OpenGL {


Texture::Texture()
	: m_handle(0)
{
	gl::GenTextures(1, &m_handle);
}


Texture::Texture(GLuint handle)
	: m_handle(handle)
{}




Texture::Texture(Texture&& other)
	: m_handle(0)
{
	swap(*this, other);
}


Texture& Texture::operator=(Texture&& other)
{
	swap(*this, other);
	return *this;
}




Texture::~Texture()
{
	if (m_handle != 0) {
		gl::DeleteTextures(1, &m_handle);
	}
}




void swap(Texture& first, Texture& second)
{
	using std::swap;

	swap(first.m_handle, second.m_handle);
}


} // namespace OpenGL

