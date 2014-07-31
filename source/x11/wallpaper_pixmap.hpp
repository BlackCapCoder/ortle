#ifndef ORTLE_X11_WALLPAPER_PIXMAP_HPP
#define ORTLE_X11_WALLPAPER_PIXMAP_HPP


#include <X11/Xlib.h>
#include <X11/Xatom.h>




namespace X11 {


struct WallpaperPixmap {

	static bool is_compatible_atom(::Atom atom);
	static void load_atoms(::Display* display);


	WallpaperPixmap();
	WallpaperPixmap(::Display* display, ::Window root);


	operator ::Pixmap() const
	{
		return pixmap;
	}


	::Pixmap pixmap;

};


} // namespace X11


#endif

