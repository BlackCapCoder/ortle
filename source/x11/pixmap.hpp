#ifndef ORTLE_X11_PIXMAP_HPP
#define ORTLE_X11_PIXMAP_HPP


#include <X11/Xlib.h>




namespace X11 {


class Pixmap {

public:

	Pixmap();
	Pixmap(::Display* display, ::Window window, unsigned int width, unsigned int height, int depth);
	Pixmap(::Display* display, ::Pixmap pixmap);

	Pixmap(Pixmap&& other);
	Pixmap& operator=(Pixmap&& other);

	~Pixmap();

	friend void swap(Pixmap& first, Pixmap& second);


public:

	operator ::Pixmap() const
	{
		return m_pixmap;
	}


private:

	::Display* m_display;
	::Pixmap m_pixmap;

};


} // namespace X11


#endif

