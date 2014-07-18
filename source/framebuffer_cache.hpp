#ifndef ORTLE_FRAMEBUFFER_CACHE_HPP
#define ORTLE_FRAMEBUFFER_CACHE_HPP


#include "opengl/core330.hpp"

#include <X11/Xlib.h>

#include <GL/glx.h>

#include <map>




class FramebufferCache {

public:

	FramebufferCache(Display* display, int screen);

	FramebufferCache(FramebufferCache&& other);
	FramebufferCache& operator=(FramebufferCache&& other);

	~FramebufferCache();

	friend void swap(FramebufferCache& first, FramebufferCache& second);


public:

	GLXFBConfig choose(int const* attributes);

	GLXFBConfig find(VisualID visual_id, int depth);


private:

	using Table = std::map<VisualID, GLXFBConfig>;


private:

	Display* m_display;
	int m_screen;
	GLXFBConfig* m_framebuffers;
	int m_count;
	Table m_table;

};



#endif
