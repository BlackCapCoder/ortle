#ifndef ORTLE_X11_EXTENSION_HPP
#define ORTLE_X11_EXTENSION_HPP


#include <X11/Xlib.h>




namespace X11 {


struct Extension {

	using QueryFunction = Bool (*)(::Display*, int*, int*);

	Extension(::Display* display, char const* name, int minimum_major, int minimum_minor, QueryFunction query_extension, QueryFunction query_version);


	int event_base;
	int error_base;

};


} // namespace X11


#endif

