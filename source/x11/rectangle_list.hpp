#ifndef ORTLE_X11_RECTANGLE_LIST_HPP
#define ORTLE_X11_RECTANGLE_LIST_HPP


#include <X11/Xlib.h>




namespace X11 {


class RectangleList {

public:

	RectangleList();
	RectangleList(::Display* display, ::Window window);

	RectangleList(RectangleList&& other);
	RectangleList& operator=(RectangleList&& other);

	~RectangleList();

	friend void swap(RectangleList& first, RectangleList& second);


public:

	::XRectangle const* begin() const
	{
		return m_rectangles;
	}

	::XRectangle const* end() const
	{
		return m_rectangles + m_count;
	}

	int size() const
	{
		return m_count;
	}


private:

	::Display* m_display;
	::Window m_window;

	int m_count;
	::XRectangle* m_rectangles;

};


} // namespace X11


#endif

