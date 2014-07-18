#include "window.hpp"

#include "exceptions.hpp"

#include <X11/Xlib.h>

#include <cassert>

#include <utility>




namespace X11 {


Window::Window()
	: m_display(nullptr)
	, m_window(None)
{}


Window::Window(
	::Display* display, ::Window parent,
	int x, int y,
	unsigned int width, unsigned int height,
	unsigned int border_width,
	int depth,
	int c_class,
	::Visual* visual,
	unsigned long attribute_mask, 
	::XSetWindowAttributes& attributes
)
	: m_display(display)
	, m_window(None)
{
	assert(display != nullptr);
	assert(parent != None);
	assert(visual != nullptr);


	::Window window = XCreateWindow(display, parent, x, y, width, height, border_width, depth, c_class, visual, attribute_mask, &attributes);

	if (window == None) {
		throw InitializationError("Could not create a new X window.");
	}

	m_window = window;
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
		XDestroyWindow(m_display, m_window);
	}
}




void swap(Window& first, Window& second)
{
	using std::swap;

	swap(first.m_display, second.m_display);
	swap(first.m_window, second.m_window);
}


} // namespace X11

