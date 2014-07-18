#ifndef ORTLE_X11_VISUAL_INFO_HPP
#define ORTLE_X11_VISUAL_INFO_HPP


#include "../opengl/core330.hpp"

#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include <GL/glx.h>




namespace X11 {


class VisualInfo {

public:

	VisualInfo(::Display* display, long mask, ::XVisualInfo& temp);
	VisualInfo(::Display* display, ::GLXFBConfig framebuffer);

	VisualInfo(VisualInfo&& other);
	VisualInfo& operator=(VisualInfo&& other);

	~VisualInfo();

	friend void swap(VisualInfo& first, VisualInfo& second);


public:

	operator ::XVisualInfo*() const
	{
		return m_visual_info;
	}

	::XVisualInfo* operator->() const
	{
		return m_visual_info;
	}


private:

	::XVisualInfo* m_visual_info;

};


} // namespace X11


#endif

