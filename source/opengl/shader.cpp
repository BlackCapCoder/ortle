#include "shader.hpp"

#include "core330.hpp"
#include "exceptions.hpp"

#include <cassert>

#include <utility>
#include <vector>



namespace {


GLint compile_status(GLuint shader)
{
	GLint status = 0;
	gl::GetShaderiv(shader, gl::COMPILE_STATUS, &status);
	return status;
}


std::vector<GLchar> info_log(GLuint shader)
{
	GLint length = 0;
	gl::GetShaderiv(shader, gl::INFO_LOG_LENGTH, &length);

	std::vector<GLchar> result(length + 1);
	gl::GetShaderInfoLog(shader, length, NULL, result.data());

	return result;
}


} // namespace 




namespace OpenGL {


Shader::Shader(GLuint handle)
	: m_handle(handle)
{}


Shader::Shader(GLenum type, char const* source)
	: m_handle(0)
{
	assert(type == gl::FRAGMENT_SHADER || type == gl::GEOMETRY_SHADER || type == gl::VERTEX_SHADER);
	assert(source != nullptr);


	GLuint handle = gl::CreateShader(type);

	gl::ShaderSource(handle, 1, &source, NULL);

	gl::CompileShader(handle);

	
	GLint status = compile_status(handle);

	if (!status) {
		auto log = info_log(handle);
		gl::DeleteShader(handle);
		throw ShaderError(log.data());
	}

	m_handle = handle;
}




Shader::Shader(Shader&& other)
	: m_handle(0)
{
	swap(*this, other);
}


Shader& Shader::operator=(Shader&& other)
{
	swap(*this, other);
	return *this;
}




Shader::~Shader()
{
	if (m_handle != 0) {
		gl::DeleteShader(m_handle);
	}
}




void swap(Shader& first, Shader& second)
{
	using std::swap;

	swap(first.m_handle, second.m_handle);
}


} // namespace OpenGL

