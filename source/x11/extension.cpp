#include "extension.hpp"

#include "exceptions.hpp"

#include <X11/Xlib.h>

#include <cassert>

#include <utility>




namespace X11 {


Extension::Extension(::Display* display, char const* name, int minimum_major, int minimum_minor, QueryFunction query_extension, QueryFunction query_version)
{
	assert(display != nullptr);


	if (!query_extension(display, &event_base, &error_base)) {
		throw MissingExtension(name);
	}


	// the version checking is a little unusual, in that it assumes that
	// major versions are backward-compatible.  this - i think - is true
	// for XFixes, but may not be for others.  i make the guess that this
	// is safe for other extensions as well due to an assumed reluctance
	// on the part of the X developers to break existing programs.

	int major = 0;
	int minor = 0;

	query_version(display, &major, &minor);

	if (major < minimum_major || (major == minimum_major && minor < minimum_minor)) {
		throw IncompatibleVersion(name);
	}
}


} // namespace X11

