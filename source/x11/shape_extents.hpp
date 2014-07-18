#ifndef ORTLE_X11_SHAPE_EXTENTS_HPP
#define ORTLE_X11_SHAPE_EXTENTS_HPP


#include <X11/Xlib.h>




namespace X11 {


struct ShapeExtents {

	ShapeExtents(::Display* display, ::Window target);


	::Window window;

	Bool bounding_shaped;

	int bounding_x;
	int bounding_y;

	unsigned int bounding_width;
	unsigned int bounding_height;


	Bool clip_shaped;

	int clip_x;
	int clip_y;

	unsigned int clip_width;
	unsigned int clip_height;

};


} // namespace X11


#endif

