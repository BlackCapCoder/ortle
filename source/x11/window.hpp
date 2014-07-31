#ifndef ORTLE_X11_WINDOW_HPP
#define ORTLE_X11_WINDOW_HPP


#include <X11/Xlib.h>




namespace X11 {


class Window {

public:

	Window();
	Window(
		::Display* display, ::Window parent,
		int x, int y,
		unsigned int width, unsigned int height,
		unsigned int border_width,
		int depth,
		int c_class,
		::Visual* visual,
		unsigned long attribute_mask,
		::XSetWindowAttributes& attributes
	);

	Window(Window&& other);
	Window& operator=(Window&& other);

	~Window();

	friend void swap(Window& first, Window& second);


public:

	operator ::Window() const
	{
		return m_window;
	}


private:

	::Display* m_display;
	::Window m_window;

};


} // namespace X11


#endif

