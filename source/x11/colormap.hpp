#ifndef ORTLE_X11_COLORMAP_HPP
#define ORTLE_X11_COLORMAP_HPP


#include <X11/Xlib.h>



namespace X11 {


class Colormap {

public:

	Colormap();
	Colormap(::Display* display, ::Window root, ::Visual* visual);

	Colormap(Colormap&& other);
	Colormap& operator=(Colormap&& other);

	~Colormap();

	friend void swap(Colormap& first, Colormap& second);


public:

	operator ::Colormap() const
	{
		return m_colormap;
	}


private:

	::Display* m_display;
	::Colormap m_colormap;

};


} // namespace X11


#endif

