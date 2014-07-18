#ifndef ORTLE_GLX_WINDOW_HPP
#define ORTLE_GLX_WINDOW_HPP


#include "../opengl/core330.hpp"

#include <X11/Xlib.h>

#include <GL/glx.h>




namespace GLX {


class Window {

public:

	Window();
	Window(::Display* display, ::GLXFBConfig framebuffer, ::Window window);

	Window(Window&& other);
	Window& operator=(Window&& other);

	~Window();

	friend void swap(Window& first, Window& second);


public:

	operator ::GLXWindow() const
	{
		return m_glx_window;
	}


private:

	::Display* m_display;
	::GLXWindow m_glx_window;

};


} // namespace GLX


#endif

