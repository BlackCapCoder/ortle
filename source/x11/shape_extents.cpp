#include "shape_extents.hpp"

#include <X11/Xlib.h>
#include <X11/extensions/shape.h>

#include <cassert>




namespace X11 {


ShapeExtents::ShapeExtents(::Display* display, ::Window target)
	: window(target)
{
	assert(display != nullptr);
	assert(target != None);

	
	XShapeQueryExtents(
		display, target,
		&bounding_shaped, &bounding_x, &bounding_y, &bounding_width, &bounding_height,
		&clip_shaped, &clip_x, &clip_y, &clip_width, &clip_height
	);
}


} // namespace X11


