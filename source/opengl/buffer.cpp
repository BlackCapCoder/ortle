#include "buffer.hpp"

#include "core330.hpp"

#include <cassert>

#include <utility>




namespace OpenGL {


Buffer::Buffer()
	: m_handle(0)
{
	gl::GenBuffers(1, &m_handle);
}


Buffer::Buffer(GLuint handle)
	: m_handle(handle)
{}




Buffer::Buffer(Buffer&& other)
	: m_handle(0)
{
	swap(*this, other);
}


Buffer& Buffer::operator=(Buffer&& other)
{
	swap(*this, other);
	return *this;
}




Buffer::~Buffer()
{
	if (m_handle != 0) {
		gl::DeleteBuffers(1, &m_handle);
	}
}




void swap(Buffer& first, Buffer& second)
{
	using std::swap;

	swap(first.m_handle, second.m_handle);
}


} // namespace OpenGL

