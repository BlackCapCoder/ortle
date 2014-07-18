#include "wallpaper_pixmap.hpp"

#include "exceptions.hpp"

#include <X11/Xlib.h>
#include <X11/Xatom.h>

#include <cassert>

#include <utility>




namespace {


::Atom l_esetroot_pmap_id = None;
::Atom l_xrootpmap_id = None;
::Atom l_xsetroot_id = None;




::Pixmap root_window_pixmap_from_atom(::Display* display, ::Window root, ::Atom atom)
{
	::Pixmap result = None;

	if (atom != None) {
		::Atom type;
		int format;
		unsigned long count;
		unsigned long bytes;
		unsigned char* data;
		if (XGetWindowProperty(display, root, atom, 0l, 1l, False, AnyPropertyType, &type, &format, &count, &bytes, &data) == Success) {
			if ((type == XA_PIXMAP) && (format == 32) && (count == 1l) && (bytes == 0l)) {
				result = *(reinterpret_cast<::Pixmap*>(data));
			}
			XFree(data);
		}
	}

	return result;
}


::Pixmap root_window_pixmap(::Display* display, ::Window root)
{
	::Pixmap result = None;

	if (l_esetroot_pmap_id != None) {
		result = root_window_pixmap_from_atom(display, root, l_esetroot_pmap_id);
	}

	if ((result == None) && (l_xrootpmap_id != None)) {
		result = root_window_pixmap_from_atom(display, root, l_xrootpmap_id);
	}

	if ((result == None) && (l_xsetroot_id != None)) {
		result = root_window_pixmap_from_atom(display, root, l_xsetroot_id);
	}

	return result;
}


} // namespace




namespace X11 {


bool WallpaperPixmap::is_compatible_atom(::Atom atom)
{
	if ((atom != None) && (atom == l_esetroot_pmap_id || atom == l_xrootpmap_id || atom == l_xsetroot_id)) {
		return true;
	}
	else {
		return false;
	}
}


void WallpaperPixmap::load_atoms(::Display* display)
{
	l_esetroot_pmap_id = XInternAtom(display, "ESETROOT_PMAP_ID", True);
	l_xrootpmap_id = XInternAtom(display, "_XROOTPMAP_ID", True);
	l_xsetroot_id = XInternAtom(display, "_XSETROOT_ID", True);
}




WallpaperPixmap::WallpaperPixmap()
	: pixmap(None)
{}


WallpaperPixmap::WallpaperPixmap(::Display* display, ::Window root)
	: pixmap(None)
{
	assert(display != nullptr);
	assert(root != None);


	pixmap = root_window_pixmap(display, root);
}


} // namespace X11

