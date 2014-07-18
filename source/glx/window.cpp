#include "window.hpp"

#include "exceptions.hpp"

#include "../opengl/core330.hpp"

#include <X11/Xlib.h>

#include <GL/glx.h>

#include <cassert>

#include <utility>




namespace GLX {


Window::Window()
	: m_display(nullptr)
	, m_glx_window(None)
{}


Window::Window(::Display* display, ::GLXFBConfig framebuffer, ::Window window)
	: m_display(display)
	, m_glx_window(None)
{
	assert(display != nullptr);
	assert(framebuffer != nullptr);
	assert(window != None);


	::GLXWindow glx_window = glXCreateWindow(display, framebuffer, window, NULL);

	if (!glx_window) {
		throw InitializationError("Could not create a new glX Window.");
	}

	m_glx_window = glx_window;
}




Window::Window(Window&& other)
	: Window()
{
	swap(*this, other);
}


Window& Window::operator=(Window&& other)
{
	swap(*this, other);
	return *this;
}




Window::~Window()
{
	if (m_display != nullptr) {
		glXDestroyWindow(m_display, m_glx_window);
	}
}




void swap(Window& first, Window& second)
{
	using std::swap;

	swap(first.m_display, second.m_display);
	swap(first.m_glx_window, second.m_glx_window);
}


} // namespace GLX

