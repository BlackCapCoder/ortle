#include "pixmap.hpp"

#include "exceptions.hpp"

#include <X11/Xlib.h>

#include <cassert>

#include <utility>




namespace X11 {


Pixmap::Pixmap()
	: m_display(nullptr)
	, m_pixmap(None)
{}


Pixmap::Pixmap(::Display* display, ::Window window, unsigned int width, unsigned int height, int depth)
	: m_display(display)
	, m_pixmap(None)
{
	assert(display != nullptr);
	assert(window != None);
	assert(depth > 0);


	::Pixmap pixmap = XCreatePixmap(display, window, width, height, depth);

	if (!pixmap) {
		throw InitializationError("Could not create a new X Pixmap.");
	}

	m_pixmap = pixmap;
}


Pixmap::Pixmap(::Display* display, ::Pixmap pixmap)
	: m_display(display)
	, m_pixmap(pixmap)
{
	assert(display != nullptr);
}




Pixmap::Pixmap(Pixmap&& other)
	: Pixmap()
{
	swap(*this, other);
}


Pixmap& Pixmap::operator=(Pixmap&& other)
{
	swap(*this, other);
	return *this;
}




Pixmap::~Pixmap()
{
	if (m_display != nullptr && m_pixmap != None) {
		XFreePixmap(m_display, m_pixmap);
	}
}




void swap(Pixmap& first, Pixmap& second)
{
	using std::swap;

	swap(first.m_display, second.m_display);
	swap(first.m_pixmap, second.m_pixmap);
}


} // namespace X11

