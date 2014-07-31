#include "program.hpp"

#include "core330.hpp"
#include "exceptions.hpp"
#include "shader.hpp"

#include <cassert>

#include <initializer_list>
#include <utility>
#include <vector>



namespace {


GLint link_status(GLuint program)
{
	GLint status = 0;
	gl::GetProgramiv(program, gl::LINK_STATUS, &status);
	return status;
}


std::vector<GLchar> info_log(GLuint program)
{
	GLint length = 0;
	gl::GetProgramiv(program, gl::INFO_LOG_LENGTH, &length);

	std::vector<GLchar> result(length + 1);
	gl::GetProgramInfoLog(program, length, NULL, result.data());

	return result;
}


} // namespace




namespace OpenGL {


Program::Program(GLuint handle)
	: m_handle(handle)
{}


Program::Program(std::initializer_list<Shader*> shaders)
	: m_handle(0)
{
	GLuint handle = gl::CreateProgram();

	for (auto it = shaders.begin(); it != shaders.end(); ++it) {
		assert(*it != nullptr);
		gl::AttachShader(handle, **it);
	}

	gl::LinkProgram(handle);

	for (auto it = shaders.begin(); it != shaders.end(); ++it) {
		gl::DetachShader(handle, **it);
	}

	GLint status = link_status(handle);

	if (!status) {
		auto log = info_log(handle);
		gl::DeleteProgram(handle);
		throw ProgramError(log.data());
	}

	m_handle = handle;
}




Program::Program(Program&& other)
	: m_handle(0)
{
	swap(*this, other);
}


Program& Program::operator=(Program&& other)
{
	swap(*this, other);
	return *this;
}




Program::~Program()
{
	if (m_handle != 0) {
		gl::DeleteProgram(m_handle);
	}
}




void swap(Program& first, Program& second)
{
	using std::swap;

	swap(first.m_handle, second.m_handle);
}


} // namespace OpenGL

