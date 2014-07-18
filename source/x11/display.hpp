#ifndef ORTLE_X11_DISPLAY_HPP
#define ORTLE_X11_DISPLAY_HPP


#include <X11/Xlib.h>




namespace X11 {


class Display {

public:

	explicit Display(char const* name);

	Display(Display&& other);
	Display& operator=(Display&& other);

	~Display();

	friend void swap(Display& first, Display& second);


public:

	operator ::Display*() const
	{
		return m_display;
	}


private:

	::Display* m_display;

};


} // namespace X11


#endif

