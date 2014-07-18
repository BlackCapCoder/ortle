#include "vertex_array.hpp"

#include "core330.hpp"

#include <cassert>

#include <utility>




namespace OpenGL {


VertexArray::VertexArray()
	: m_handle(0)
{
	gl::GenVertexArrays(1, &m_handle);
}


VertexArray::VertexArray(GLuint handle)
	: m_handle(handle)
{}




VertexArray::VertexArray(VertexArray&& other)
	: m_handle(0)
{
	swap(*this, other);
}


VertexArray& VertexArray::operator=(VertexArray&& other)
{
	swap(*this, other);
	return *this;
}




VertexArray::~VertexArray()
{
	if (m_handle != 0) {
		gl::DeleteVertexArrays(1, &m_handle);
	}
}




void swap(VertexArray& first, VertexArray& second)
{
	using std::swap;

	swap(first.m_handle, second.m_handle);
}


} // namespace OpenGL

