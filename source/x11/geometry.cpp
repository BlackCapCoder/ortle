#include "geometry.hpp"

#include "../utility/trace.hpp"

#include <X11/Xlib.h>

#include <cassert>




namespace X11 {


Geometry::Geometry(::Display* display, ::Drawable target, ::Window relative)
	: drawable(target)
	, root(None)
	, x(0)
	, y(0)
	, width(0)
	, height(0)
	, border_width(0)
	, depth(0)
{
	assert(display != nullptr);
	assert(target != None);


	if (!XGetGeometry(display, target, &root, &x, &y, &width, &height, &border_width, &depth)) {
		TRACE("WARNING", "failed to get geometry for drawable", target);
		return;
	}

	if (relative != None) {
		Window dummy = None;
		XTranslateCoordinates(display, target, relative, -border_width, -border_width, &x, &y, &dummy);
	}
}


} // namespace X11

