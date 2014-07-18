#ifndef ORTLE_X11_GEOMETRY_HPP
#define ORTLE_X11_GEOMETRY_HPP


#include <X11/Xlib.h>




namespace X11 {


struct Geometry {

	Geometry(::Display* display, ::Window target, ::Window relative = None);


	::Window window;
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

