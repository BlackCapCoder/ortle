#ifndef ORTLE_GLX_CONTEXT_HPP
#define ORTLE_GLX_CONTEXT_HPP


#include "../opengl/core330.hpp"

#include <X11/Xlib.h>

#include <GL/glx.h>




namespace GLX {


class Context {

public:

	Context();
	Context(::Display* display, ::GLXFBConfig framebuffer, int const* attributes);

	Context(Context&& other);
	Context& operator=(Context&& other);

	~Context();

	friend void swap(Context& first, Context& second);


public:

	operator ::GLXContext() const
	{
		return m_glx_context;
	}


private:

	::Display* m_display;
	::GLXContext m_glx_context;

};


} // namespace GLX


#endif

