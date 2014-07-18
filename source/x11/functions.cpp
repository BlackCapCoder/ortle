#include "functions.hpp"

#include <X11/Xlib.h>
#include <X11/extensions/shape.h>
#include <X11/extensions/Xfixes.h>




namespace X11 {


void set_click_through(::Display* display, ::Window window)
{
	XserverRegion region = XFixesCreateRegion(display, NULL, 0);
	XFixesSetWindowShapeRegion(display, window, ShapeInput, 0, 0, region);
	XFixesDestroyRegion(display, region);
}


} // namespace X11

