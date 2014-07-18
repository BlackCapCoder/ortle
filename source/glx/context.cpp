#include "context.hpp"

#include "exceptions.hpp"
#include "functions.hpp"

#include "../opengl/core330.hpp"

#include <X11/Xlib.h>

#include <GL/glx.h>

#include <cassert>

#include <utility>




namespace GLX {


Context::Context()
	: m_display(nullptr)
	, m_glx_context(nullptr)
{}


Context::Context(::Display* display, ::GLXFBConfig framebuffer, int const* attributes)
	: m_display(display)
	, m_glx_context(nullptr)
{
	assert(display != nullptr);
	assert(framebuffer != nullptr);
	assert(CreateContextAttribsARB != nullptr);


	::GLXContext glx_context = CreateContextAttribsARB(display, framebuffer, NULL, True, attributes);

	if (!glx_context) {
		throw InitializationError("Could not create a new glX Context.");
	}

	m_glx_context = glx_context;
}




Context::Context(Context&& other)
	: Context()
{
	swap(*this, other);
}


Context& Context::operator=(Context&& other)
{
	swap(*this, other);
	return *this;
}




Context::~Context()
{
	if (m_display != nullptr) {
		glXDestroyContext(m_display, m_glx_context);
	}
}




void swap(Context& first, Context& second)
{
	using std::swap;

	swap(first.m_display, second.m_display);
	swap(first.m_glx_context, second.m_glx_context);
}


} // namespace GLX

