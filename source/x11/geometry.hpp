#ifndef ORTLE_X11_GEOMETRY_HPP
#define ORTLE_X11_GEOMETRY_HPP


#include <X11/Xlib.h>




namespace X11 {


struct Geometry {

	Geometry(::Display* display, ::Drawable target, ::Window relative = None);


	::Drawable drawable;
	::Window root;

	int x;
	int y;

	unsigned int width;
	unsigned int height;
	unsigned int border_width;

	unsigned int depth;

};


} // namespace X11


#endif

