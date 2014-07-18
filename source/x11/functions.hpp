#ifndef ORTLE_X11_FUNCTIONS_HPP
#define ORTLE_X11_FUNCTIONS_HPP


#include <X11/Xlib.h>




namespace X11 {


/// set_click_through

/// Sets the ShapeInput region of the given window to an empty region, thereby 
/// disabling mouse input.

void set_click_through(::Display* display, ::Window window);


} // namespace X11


#endif

