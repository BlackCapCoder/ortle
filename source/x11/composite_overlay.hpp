#ifndef ORTLE_X11_COMPOSITE_OVERLAY_HPP
#define ORTLE_X11_COMPOSITE_OVERLAY_HPP


#include <X11/Xlib.h>




namespace X11 {


class CompositeOverlay {

public:

	CompositeOverlay(::Display* display, ::Window root);

	CompositeOverlay(CompositeOverlay&& other);
	CompositeOverlay& operator=(CompositeOverlay&& other);

	~CompositeOverlay();

	friend void swap(CompositeOverlay& first, CompositeOverlay& second);


public:

	operator ::Window() const
	{
		return m_composite_overlay;
	}


private:

	::Display* m_display;
	::Window m_root;
	::Window m_composite_overlay;

};


} // namespace X11


#endif

