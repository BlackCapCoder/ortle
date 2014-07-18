#include "geometry.hpp"

#include <X11/Xlib.h>

#include <cassert>




namespace X11 {


Geometry::Geometry(::Display* display, ::Window target, ::Window relative)
	: window(target)
{
	assert(display != nullptr);
	assert(target != None);
	assert(relative != None);


	XGetGeometry(display, target, &root, &x, &y, &width, &height, &border_width, &depth);

	if (relative != None) {
		Window dummy = None;
		XTranslateCoordinates(display, target, relative, -border_width, -border_width, &x, &y, &dummy);
	}
}


} // namespace X11

