#include "output_window.hpp"

#include "exceptions.hpp"
#include "framebuffer_cache.hpp"

#include "glx/context.hpp"
#include "glx/functions.hpp"
#include "glx/window.hpp"

#include "opengl/core330.hpp"

#include "utility/trace.hpp"

#include "x11/colormap.hpp"
#include "x11/functions.hpp"
#include "x11/geometry.hpp"
#include "x11/visual_info.hpp"
#include "x11/window.hpp"

#include <X11/Xlib.h>
#include <X11/extensions/shape.h>
#include <X11/extensions/Xfixes.h>

#include <GL/glx.h>

#include <cassert>

#include <utility>




namespace {


int const l_context_attributes[] = {

	GLX_CONTEXT_MAJOR_VERSION_ARB, 3,
	GLX_CONTEXT_MINOR_VERSION_ARB, 3,

	GLX_CONTEXT_PROFILE_MASK_ARB,  GLX_CONTEXT_CORE_PROFILE_BIT_ARB,

	GLX_RENDER_TYPE,               GLX_RGBA_TYPE,

	None

};


int const l_framebuffer_attributes[] = {

	GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
	GLX_RENDER_TYPE,   GLX_RGBA_BIT,

	GLX_DOUBLEBUFFER,  True,

	GLX_RED_SIZE,      8,
	GLX_GREEN_SIZE,    8,
	GLX_BLUE_SIZE,     8,
	GLX_ALPHA_SIZE,    8,

	None

};


} // namespace




OutputWindow::OutputWindow(Display* display, Window root, Window parent, FramebufferCache& framebuffers)
	: m_display(display)
	, m_root(root)
	, m_parent(parent)
	, m_colormap()
	, m_window()
	, m_glx_window()
	, m_glx_context()
{
	assert(display != nullptr);
	assert(root != None);
	assert(parent != None);


	TRACE("creating output window on root", root);

	// load glx functions

	GLX::load_functions();
	

	// get a framebuffer.  this may throw.

	GLXFBConfig framebuffer = framebuffers.choose(l_framebuffer_attributes);


	// find that framebuffer's visual info.  this may also throw.

	X11::VisualInfo visual_info(display, framebuffer);


	// create a colormap for the new window
	// this might throw if allocating a new colormap fails

	m_colormap = X11::Colormap(display, root, visual_info->visual);


	// get the geometry of the root window
	// this will give us the initial width and height of our window

	X11::Geometry root_geometry(display, root);


	// get the geometry of the parent window
	// this will give us the initial x and y coordinates for our window

	X11::Geometry parent_geometry(display, parent, root);


	// create the new window

	XSetWindowAttributes window_attributes;
	window_attributes.colormap = m_colormap;
	window_attributes.event_mask = StructureNotifyMask;
	window_attributes.override_redirect = True;

	unsigned long window_attributes_mask = CWColormap | CWEventMask | CWOverrideRedirect;

	// X11Window window(display, parent, 0, 0, 640, 480, 0, visual_info->depth, InputOutput, visual_info->visual, window_attributes_mask, window_attributes);
	m_window = X11::Window(display, parent, -parent_geometry.x, -parent_geometry.y, root_geometry.width, root_geometry.height, 0, visual_info->depth, InputOutput, visual_info->visual, window_attributes_mask, window_attributes);


	// create a glx window

	m_glx_window = GLX::Window(display, framebuffer, m_window);


	// create a glx context

	m_glx_context = GLX::Context(display, framebuffer, l_context_attributes);


	// map the window

	XMapWindow(display, m_window);


	// wait for the map to happen

	XEvent event;
	while(XNextEvent(display, &event)) {
		if (event.type == MapNotify) {
			XMapEvent& map_event = reinterpret_cast<XMapEvent&>(event);
			if (map_event.window == m_window) {
				break;
			}
		}
	}


	// make the context current

	make_context_current();


	// load opengl functions

	if (!gl::sys::LoadFunctions()) {
		throw InitializationError("Could not load OpenGL functions.");
	}


	// disable the mouse

	X11::set_click_through(display, parent);
	X11::set_click_through(display, m_window);
}




OutputWindow::OutputWindow(OutputWindow&& other)
	: m_display(nullptr)
	, m_root(None)
	, m_parent(None)
	, m_colormap()
	, m_window()
	, m_glx_window()
	, m_glx_context()
{
	swap(*this, other);
}


OutputWindow& OutputWindow::operator=(OutputWindow&& other)
{
	swap(*this, other);
	return *this;
}




OutputWindow::~OutputWindow()
{
	TRACE("destroying output window for root", m_root);

	glXMakeContextCurrent(m_display, None, None, NULL);
}




void swap(OutputWindow& first, OutputWindow& second)
{
	using std::swap;

	swap(first.m_display, second.m_display);
	swap(first.m_root, second.m_root);
	swap(first.m_parent, second.m_parent);
	
	swap(first.m_colormap, second.m_colormap);
	swap(first.m_window, second.m_window);

	swap(first.m_glx_window, second.m_glx_window);
	swap(first.m_glx_context, second.m_glx_context);
}




void OutputWindow::reconfigure()
{
	assert(m_display != nullptr);


	// get the geometry of the root window
	// this will give us the initial width and height of our window

	X11::Geometry root_geometry(m_display, m_root);


	// get the geometry of the parent window
	// this will give us the initial x and y coordinates for our window

	X11::Geometry parent_geometry(m_display, m_parent, m_root);



	// actually move and resize the window

	set_position(-parent_geometry.x, -parent_geometry.y);
	set_size(root_geometry.width, root_geometry.height);
}


void OutputWindow::set_size(unsigned int width, unsigned int height)
{
	assert(m_display != nullptr);

	XResizeWindow(m_display, m_window, width, height);
}


void OutputWindow::set_position(int x, int y)
{
	assert(m_display != nullptr);

	XMoveWindow(m_display, m_window, x, y);
}




void OutputWindow::make_context_current()
{
	assert(m_display != nullptr);

	glXMakeContextCurrent(m_display, m_glx_window, m_glx_window, m_glx_context);
}


void OutputWindow::swap_buffers()
{
	assert(m_display != nullptr);

	glXSwapBuffers(m_display, m_glx_window);
}


void OutputWindow::swap_interval(int interval)
{
	assert(m_display != nullptr);

	if (GLX::SwapIntervalEXT) {
		GLX::SwapIntervalEXT(m_display, m_glx_window, interval);
	}
	else if (GLX::SwapIntervalMESA) {
		GLX::SwapIntervalMESA(interval);
	}
	else {
		throw InitializationError("GLX extension GLX_MESA_swap_control or GLX_EXT_swap_control required.");
	}
}

