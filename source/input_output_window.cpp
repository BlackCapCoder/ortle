#include "input_output_window.hpp"

#include "exceptions.hpp"
#include "framebuffer_cache.hpp"
#include "managed_window.hpp"
#include "renderer.hpp"

#include "glx/functions.hpp"
#include "glx/pixmap.hpp"

#include "opengl/core330.hpp"
#include "opengl/texture.hpp"

#include "utility/trace.hpp"

#include "x11/exceptions.hpp"
#include "x11/rectangle_list.hpp"
#include "x11/pixmap.hpp"
#include "x11/shape_extents.hpp"

#include <X11/Xlib.h>
#include <X11/extensions/shape.h>
#include <X11/extensions/Xcomposite.h>

#include <GL/glx.h>

#include <cassert>

#include <utility>




InputOutputWindow::InputOutputWindow(Display* display, Window root, XCreateWindowEvent const& event, XWindowAttributes const& attributes, FramebufferCache& framebuffers)
	: ManagedWindow(event.window)
	, m_display(display)
	, m_root(root)
	, m_framebuffer(framebuffers.find(XVisualIDFromVisual(attributes.visual), attributes.depth))
	, m_pixmap()
	, m_glx_pixmap()
	, m_texture()
	, m_rectangles()
	, m_x(0)
	, m_y(0)
	, m_width(0)
	, m_height(0)
	, m_border_width(0)
	, m_rgba(GLX::framebuffer_supports_rgba(display, m_framebuffer))
	, m_shaped(false)
	, m_mapped(false)
	, m_texture_invalidated(true)
	// , m_rectangles_invalidated(true)
{
	assert(display != nullptr);
	assert(root != None);
	assert(event.window != None);
	assert(event.window != root);


	TRACE("starting management of input/output window", event.window);

	// during initialization and some ReparentNotify events, a fake
	// XCreateWindowEvent is passed to this function.  in those cases the
	// type field has been modified to reflect which case we are in.


	// case 1: this window was created in response to a natural CreateNotify
	// event.  nothing special needs to be done.

	if (event.type == CreateNotify) {
		TRACE(event.x, event.y, event.width, event.height, event.border_width);
		reconfigure(event.x, event.y, event.width, event.height, event.border_width);
	}


	// case 2: this window already exists, and was reparented to a window in
	// the stack.  the window is unmapped (the XReparentWindow docs say that
	// an UnmapWindow is automatically done), but the event does not include
	// the window's size so we need to use its attributes (which may be ahead
	// of our local state) for its dimensions and shape.

	else if (event.type == ReparentNotify) {

		X11::ShapeExtents shape_extents(display, event.window);
		if (shape_extents.bounding_shaped == True && shape_extents.bounding_width > 0 && shape_extents.bounding_height > 0) {
			m_shaped = true;
			update_shape_rectangles();
		}
		TRACE("reparent", attributes.x, attributes.y, attributes.width, attributes.height, attributes.border_width);
		reconfigure(attributes.x, attributes.y, attributes.width, attributes.height, attributes.border_width);
	}

	// case 3: we are currently in WindowManager's initialization.  the window
	// already exists, and is possibly mapped.

	else {

		X11::ShapeExtents shape_extents(display, event.window);
		if (shape_extents.bounding_shaped == True && shape_extents.bounding_width > 0 && shape_extents.bounding_height > 0) {
			m_shaped = true;
			update_shape_rectangles();
		}

		TRACE("init", attributes.x, attributes.y, attributes.width, attributes.height, attributes.border_width);
		reconfigure(attributes.x, attributes.y, attributes.width, attributes.height, attributes.border_width);

		if (attributes.map_state == IsViewable) {
			on_map_notify_impl(XMapEvent());
		}
	}
}




InputOutputWindow::InputOutputWindow(InputOutputWindow&& other)
	: ManagedWindow(None)
	, m_display(nullptr)
	, m_root(None)
	, m_framebuffer(nullptr)
	, m_pixmap()
	, m_glx_pixmap()
	, m_texture(0)
	, m_rectangles()
	, m_x(0)
	, m_y(0)
	, m_width(0)
	, m_height(0)
	, m_border_width(0)
	, m_rgba(false)
	, m_shaped(false)
	, m_mapped(false)
	, m_texture_invalidated(true)
	// , m_rectangles_invalidated(true)
{
	swap(*this, other);
}


InputOutputWindow& InputOutputWindow::operator=(InputOutputWindow&& other)
{
	swap(*this, other);
	return *this;
}




InputOutputWindow::~InputOutputWindow()
{
	if (m_display != nullptr) {

		TRACE("stopping management of window", static_cast<Window>(*this));

		release_and_destroy();
	}
}




void swap(InputOutputWindow& first, InputOutputWindow& second)
{
	using std::swap;

	swap(static_cast<ManagedWindow&>(first), static_cast<ManagedWindow&>(second));

	swap(first.m_display, second.m_display);
	swap(first.m_root, second.m_root);
	swap(first.m_framebuffer, second.m_framebuffer);
	swap(first.m_pixmap, second.m_pixmap);
	swap(first.m_glx_pixmap, second.m_glx_pixmap);
	swap(first.m_texture, second.m_texture);
	swap(first.m_rectangles, second.m_rectangles);
	swap(first.m_x, second.m_x);
	swap(first.m_y, second.m_y);
	swap(first.m_width, second.m_width);
	swap(first.m_height, second.m_height);
	swap(first.m_border_width, second.m_border_width);
	swap(first.m_rgba, second.m_rgba);
	swap(first.m_shaped, second.m_shaped);
	swap(first.m_mapped, second.m_mapped);
	swap(first.m_texture_invalidated, second.m_texture_invalidated);
	// swap(first.m_rectangles_invalidated, second.m_rectangles_invalidated);
}




void InputOutputWindow::render_impl(Renderer& renderer)
{
	// first, check that this window is mapped and has a pixmap.  if it is
	// mapped and _doesn't_ have a pixmap, it is likely about to be destroyed
	// or off-screen somewhere, and it doesn't need to be drawn.

	if (m_mapped && m_pixmap != None) {

		// if the texture was invalidated (say, by a resize), generate a new
		// GLXPixmap for our window's pixmap and bind the texture to it.

		if (m_texture_invalidated) {
			create_and_bind();
		}

		// bind window texture and set window uniforms

		gl::BindTexture(gl::TEXTURE_2D, m_texture);

		// TRACE("DRAWING", m_shaped, m_texture, m_x, m_y, m_width, m_height, m_border_width);

		renderer.set_border_width(static_cast<float>(m_border_width));
		renderer.set_window_geometry(
			static_cast<float>(m_x),
			static_cast<float>(m_y),
			static_cast<float>(m_width),
			static_cast<float>(m_height)
		);

		// then either draw each subrectangle if we are shaped

		if (m_shaped && m_rectangles.size() > 0) {

			for (auto it = m_rectangles.begin(); it != m_rectangles.end(); ++it) {
				renderer.set_rectangle_geometry(
					static_cast<float>(it->x),
					static_cast<float>(it->y),
					static_cast<float>(it->width),
					static_cast<float>(it->height)
				);
				renderer.draw_quad();
			}
		}

		// or just draw the whole window

		else {

			renderer.set_rectangle_geometry(
				static_cast<float>(-m_border_width),
				static_cast<float>(-m_border_width),
				static_cast<float>(2 * m_border_width + m_width),
				static_cast<float>(2 * m_border_width + m_height)
			);
			renderer.draw_quad();
		}

		gl::BindTexture(gl::TEXTURE_2D, 0);
	}
}




void InputOutputWindow::on_configure_notify_impl(XConfigureEvent const& event)
{
	reconfigure(event.x, event.y, event.width, event.height, event.border_width);
}


void InputOutputWindow::on_map_notify_impl(XMapEvent const&)
{
	// note: bind_composite_pixmap() may fail, in which case m_pixmap and
	// m_glx_pixmap will remain empty.  this needs to be taken into account in
	// render().

	bind_composite_pixmap();
	create_and_bind();

	m_mapped = true;
}


void InputOutputWindow::on_shape_notify_impl(XShapeEvent const& event)
{
	m_shaped = (event.shaped == True);
	update_shape_rectangles();
}


void InputOutputWindow::on_unmap_notify_impl(XUnmapEvent const&)
{
	m_mapped = false;

	release_and_destroy();
	release_composite_pixmap();
}




void InputOutputWindow::reconfigure(int x, int y, int width, int height, int border_width)
{
	// update the composite pixmap if we are visible and our dimensions have 
	// changed 

	// TODO: only refresh the composite pixmap if there are no future configure
	// events pending for this window

	if (m_mapped) {
		if (width != m_width || height != m_height || border_width != m_border_width) {
			bind_composite_pixmap();
		}
	}

	m_x = x;
	m_y = y;

	m_width = width;
	m_height = height;

	m_border_width = border_width;
}




void InputOutputWindow::bind_composite_pixmap()
{
	// XCompositeNameWindowPixmap may fail if the window has already been
	// destroyed or if it is - for whatever reason - not visible.

	X11::Pixmap pixmap(m_display, XCompositeNameWindowPixmap(m_display, *this));


	// case 1: we are already using this pixmap, so do nothing.

	if (pixmap == m_pixmap) {
		return;
	}


	// case 2: this is a new pixmap.  release the glx pixmap and texture
	// binding and replace our current pixmap with this one.

	else if (pixmap != None) {
		release_and_destroy();
		m_pixmap = std::move(pixmap);
	}


	// case 3: we couldn't get the composite pixmap.  this window is either
	// not visible or destroyed.

	else {
		release_composite_pixmap();
	}
}


void InputOutputWindow::release_composite_pixmap()
{
	if (m_pixmap != None) {
		release_and_destroy();
		m_pixmap = X11::Pixmap();
	}
}




void InputOutputWindow::create_and_bind()
{
	if (m_pixmap != None) {

		if (m_rgba) {
			m_glx_pixmap = GLX::Pixmap(m_display, m_framebuffer, m_pixmap, GLX::Pixmap::rgba_attributes);
		}
		else {
			m_glx_pixmap = GLX::Pixmap(m_display, m_framebuffer, m_pixmap, GLX::Pixmap::rgb_attributes);
		}

		gl::BindTexture(gl::TEXTURE_2D, m_texture);

		gl::TexParameteri(gl::TEXTURE_2D, gl::TEXTURE_MAG_FILTER, gl::LINEAR);
		gl::TexParameteri(gl::TEXTURE_2D, gl::TEXTURE_MIN_FILTER, gl::LINEAR);

		GLX::BindTexImageEXT(m_display, m_glx_pixmap, GLX_FRONT_EXT, NULL);

		gl::BindTexture(gl::TEXTURE_2D, 0);

		m_texture_invalidated = false;
	}
}


void InputOutputWindow::release_and_destroy()
{
	if (m_glx_pixmap != None) {

		gl::BindTexture(gl::TEXTURE_2D, m_texture);
		GLX::ReleaseTexImageEXT(m_display, m_glx_pixmap, GLX_FRONT_EXT);
		gl::BindTexture(gl::TEXTURE_2D, 0);

		m_glx_pixmap = GLX::Pixmap();
	}

	m_texture_invalidated = true;
}


void InputOutputWindow::update_shape_rectangles()
{
	if (m_shaped) {
		try {
			X11::RectangleList rectangles(m_display, *this);
			m_rectangles = std::move(rectangles);
		}
		catch (X11::InitializationError&) {
			TRACE("WARNING", "failed to get bounding rectangles for shaped window", *this);
		}
	}
}
