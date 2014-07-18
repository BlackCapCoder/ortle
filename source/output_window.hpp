#ifndef ORTLE_OUTPUT_WINDOW_HPP
#define ORTLE_OUTPUT_WINDOW_HPP


#include "glx/context.hpp"
#include "glx/window.hpp"

#include "x11/colormap.hpp"
#include "x11/window.hpp"

#include <X11/Xlib.h>




class FramebufferCache;


class OutputWindow {

public:

	OutputWindow(Display* display, Window root, Window parent, FramebufferCache& framebuffers);

	OutputWindow(OutputWindow&& other);
	OutputWindow& operator=(OutputWindow&& other);

	~OutputWindow();

	friend void swap(OutputWindow& first, OutputWindow& second);


public:

	operator Window() const { return m_window; }


public:

	void reconfigure();
	void set_position(int x, int y);
	void set_size(unsigned int width, unsigned int height);

	void make_context_current();
	void swap_buffers();
	void swap_interval(int interval);


private:

	Display* m_display;
	Window m_root;
	Window m_parent;

	X11::Colormap m_colormap;
	X11::Window m_window;
	
	GLX::Window m_glx_window;
	GLX::Context m_glx_context;

};



#endif

