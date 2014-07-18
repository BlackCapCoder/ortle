#include "colormap.hpp"

#include "exceptions.hpp"

#include <X11/Xlib.h>

#include <cassert>

#include <utility>




namespace X11 {


Colormap::Colormap()
	: m_display(nullptr)
	, m_colormap(None)
{}


Colormap::Colormap(::Display* display, ::Window root, ::Visual* visual)
	: m_display(display)
	, m_colormap(None)
{
	assert(display != nullptr);
	assert(root != None);
	assert(visual != nullptr);


	::Colormap colormap = XCreateColormap(display, root, visual, AllocNone);

	if (!colormap) {
		throw InitializationError("Could not create a new X colormap.");
	}

	m_colormap = colormap;
}




Colormap::Colormap(Colormap&& other)
	: Colormap()
{
	swap(*this, other);
}


Colormap& Colormap::operator=(Colormap&& other)
{
	swap(*this, other);
	return *this;
}




Colormap::~Colormap()
{
	if (m_display != nullptr) {
		XFreeColormap(m_display, m_colormap);
	}
}




void swap(Colormap& first, Colormap& second)
{
	using std::swap;

	swap(first.m_display, second.m_display);
	swap(first.m_colormap, second.m_colormap);
}


} // namespace X11

