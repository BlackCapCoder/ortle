#ifndef ORTLE_GLX_PIXMAP_HPP
#define ORTLE_GLX_PIXMAP_HPP


#include "../opengl/core330.hpp"

#include <X11/Xlib.h>

#include <GL/glx.h>




namespace GLX {

	
class Pixmap {

public:

	static int const rgb_attributes[];
	static int const rgba_attributes[];


public:

	Pixmap();
	Pixmap(::Display* display, ::GLXFBConfig framebuffer, ::Pixmap pixmap, int const* attributes);

	Pixmap(Pixmap&& other);
	Pixmap& operator=(Pixmap&& other);

	~Pixmap();

	friend void swap(Pixmap& first, Pixmap& second);


public:

	operator ::GLXPixmap() const
	{
		return m_glx_pixmap;
	}


private:

	::Display* m_display;
	::GLXPixmap m_glx_pixmap;

};


} // namespace GLX


#endif

