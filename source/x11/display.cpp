#include "display.hpp"

#include "exceptions.hpp"

#include <X11/Xlib.h>

#include <utility>




namespace X11 {


Display::Display(char const* name)
	: m_display(nullptr)
{
	::Display* display = XOpenDisplay(name);

	if (display == nullptr) {
		throw InitializationError("Could not open X display.");
	}

	m_display = display;
}




Display::Display(Display&& other)
	: m_display(nullptr)
{
	swap(*this, other);
}


Display& Display::operator=(Display&& other)
{
	swap(*this, other);
	return *this;
}




Display::~Display()
{
	if (m_display != nullptr) {
		XCloseDisplay(m_display);
	}
}




void swap(Display& first, Display& second)
{
	using std::swap;

	swap(first.m_display, second.m_display);
}


} // namespace X11

