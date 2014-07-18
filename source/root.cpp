#include "root.hpp"

#include "exceptions.hpp"
#include "framebuffer_cache.hpp"
#include "managed_window.hpp"
#include "renderer.hpp"

#include "glx/functions.hpp"
#include "glx/pixmap.hpp"

#include "opengl/core330.hpp"
#include "opengl/texture.hpp"

#include "utility/trace.hpp"

// #include "x11/damage.hpp"
#include "x11/geometry.hpp"
#include "x11/pixmap.hpp"
#include "x11/wallpaper_pixmap.hpp"

#include <X11/Xlib.h>
#include <X11/extensions/shape.h>
// #include <X11/extensions/Xdamage.h>

#include <GL/glx.h>

#include <cassert>

#include <utility>




Root::Root(Display* display, int screen, Window root, FramebufferCache& framebuffers)
	: ManagedWindow(root)
	, m_display(display)
	, m_screen(screen)
	, m_root(root)
	, m_framebuffer(framebuffers.find(XVisualIDFromVisual(XDefaultVisual(display, screen)), XDefaultDepth(display, screen)))
	// , m_damage(display, root, XDamageReportBoundingBox)
	, m_pixmap()
	, m_glx_pixmap()
	, m_texture()
	, m_width(0)
	, m_height(0)
	, m_rgba(GLX::framebuffer_supports_rgba(display, m_framebuffer))
	, m_waiting_for_success(false)
{
	assert(display != nullptr);
	assert(screen >= 0);
	assert(root != None);


	TRACE("starting management of root window", root);

	X11::WallpaperPixmap::load_atoms(display);

	X11::Geometry geometry(display, root);

	m_width = geometry.width;
	m_height = geometry.height;

	create_and_bind();
}




Root::Root(Root&& other)
	: ManagedWindow(None)
	, m_display(nullptr)
	, m_screen(0)
	, m_root(None)
	, m_framebuffer(nullptr)
	// , m_damage()
	, m_pixmap()
	, m_glx_pixmap()
	, m_texture(0)
	, m_width(0)
	, m_height(0)
	, m_rgba(false)
	, m_waiting_for_success(false)
{
	swap(*this, other);
}


Root& Root::operator=(Root&& other)
{
	swap(*this, other);
	return *this;
}




Root::~Root()
{
	if (m_display != nullptr) {
		TRACE("stopping management of root window", m_root);
	}
}




void swap(Root& first, Root& second)
{
	using std::swap;

	swap(static_cast<ManagedWindow&>(first), static_cast<ManagedWindow&>(second));

	swap(first.m_display, second.m_display);
	swap(first.m_screen, second.m_screen);
	swap(first.m_root, second.m_root);

	swap(first.m_framebuffer, second.m_framebuffer);

	// swap(first.m_damage, second.m_damage);

	swap(first.m_pixmap, second.m_pixmap);
	swap(first.m_glx_pixmap, second.m_glx_pixmap);
	swap(first.m_texture, second.m_texture);

	swap(first.m_width, second.m_width);
	swap(first.m_height, second.m_height);

	swap(first.m_rgba, second.m_rgba);
	swap(first.m_waiting_for_success, second.m_waiting_for_success);
}




void Root::render_impl(Renderer& renderer)
{
	if (m_pixmap != None && !m_waiting_for_success) {

		gl::BindTexture(gl::TEXTURE_2D, m_texture);

		renderer.set_border_width(0);

		renderer.set_window_geometry(
			static_cast<float>(0),
			static_cast<float>(0),
			static_cast<float>(m_width),
			static_cast<float>(m_height)
		);

		renderer.set_rectangle_geometry(
			static_cast<float>(0),
			static_cast<float>(0),
			static_cast<float>(m_width),
			static_cast<float>(m_height)
		);

		renderer.draw_quad();

		gl::BindTexture(gl::TEXTURE_2D, 0);
	}
}




void Root::on_configure_notify_impl(XConfigureEvent const& event)
{
	assert(m_display != nullptr);
	assert(event.window == m_root);

	if (event.width != m_width || event.height != m_height) {

		TRACE("handling resize of root window", m_root);

		m_width = event.width;
		m_height = event.height;

		release_and_destroy();
		create_and_bind();
	}
}


// void Root::on_damage_notify_impl(XDamageNotifyEvent const& event)
// {
// 	assert(m_display != nullptr);
// 	assert(event.drawable == m_root);


// 	if (m_pixmap != None) {

// 		TRACE("copying damaged area of root window", m_root, event.area.x, event.area.y, event.area.width, event.area.height);

// 		XCopyArea(
// 			m_display, m_root, m_pixmap, XDefaultGC(m_display, m_screen),
// 			static_cast<int>(event.area.x), static_cast<int>(event.area.y),
// 			static_cast<unsigned int>(event.area.width), static_cast<unsigned int>(event.area.height),
// 			static_cast<int>(event.area.x), static_cast<int>(event.area.y)
// 		);

// 		XDamageSubtract(m_display, event.damage, ...); //!!
// 	}
// }


void Root::on_graphics_expose_impl(XGraphicsExposeEvent const& event)
{
	if (event.drawable == m_pixmap) {

		// this means that the XCopyArea step of create_and_bind() failed.
		// we may as well release any resources we acquired.

		release_and_destroy();
	}
}


void Root::on_no_expose_impl(XNoExposeEvent const& event) 
{
	if (event.drawable == m_pixmap) {

		// this means that XCopyArea in create_and_bind() succeeded and we 
		// can draw this window.

		m_waiting_for_success = false;
	}
}


void Root::on_property_notify_impl(XPropertyEvent const& event)
{
	assert(m_display != nullptr);
	assert(event.window == m_root);


	if (X11::WallpaperPixmap::is_compatible_atom(event.atom)) {
		release_and_destroy();
		create_and_bind();
	}
}




void Root::create_and_bind()
{
	if (m_pixmap == None) {

		// create a pixmap the same size as the root window.  its contents
		// are undefined.

		m_pixmap = X11::Pixmap(m_display, m_root, m_width, m_height, XDefaultDepth(m_display, m_screen));


		// look up the wallpaper pixmap (e.g. _XSETROOT_ID).  note that this is 
		// just a random property set on the root window and doesn't 
		// necessarily even refer to a pixmap.  

		X11::WallpaperPixmap wallpaper(m_display, m_root);


		// in spite of the previous comment, this check is still somewhat valid 
		// as X11::WallpaperPixmap will contain None if none of the atoms are 
		// set.

		if (wallpaper == None) {
			m_pixmap = X11::Pixmap();
			return;
		}

		TRACE("copying root pixmap", "source", wallpaper, "target", m_pixmap);


		// copy the wallpaper pixmap into our pixmap.  note that we have no way
		// of checking if this succeeds.  we instead rely on a generated 
		// NoExpose event to tell us that it is okay to draw anything.

		XCopyArea(
			m_display, wallpaper, m_pixmap, XDefaultGC(m_display, m_screen),
			0, 0, m_width, m_height, 0, 0
		);

		m_waiting_for_success = true;


		// okay, so, there can potentially be stuff that was drawn directly 
		// onto the root window - conky comes to mind - but this XCopyArea 
		// never, ever works.  i get a GraphicsExpose event saying that none 
		// of the area is available to copy.  which makes sense because the 
		// root window _is_ obscured by my output window and can't be 
		// redirected with XComposite.

		// i'm not sure if there's a way around this.

		// XCopyArea(
		// 	m_display, m_root, m_pixmap, XDefaultGC(m_display, m_screen),
		// 	0, 0, m_width, m_height, 0, 0
		// );


		// create a glx pixmap for our pixmap

		if (m_rgba) {
			m_glx_pixmap = GLX::Pixmap(m_display, m_framebuffer, m_pixmap, GLX::Pixmap::rgba_attributes);
		}
		else {
			m_glx_pixmap = GLX::Pixmap(m_display, m_framebuffer, m_pixmap, GLX::Pixmap::rgb_attributes);
		}


		// bind the glx pixmap to our texture

		gl::BindTexture(gl::TEXTURE_2D, m_texture);

		gl::TexParameteri(gl::TEXTURE_2D, gl::TEXTURE_MAG_FILTER, gl::LINEAR);
		gl::TexParameteri(gl::TEXTURE_2D, gl::TEXTURE_MIN_FILTER, gl::LINEAR);

		GLX::BindTexImageEXT(m_display, m_glx_pixmap, GLX_FRONT_EXT, NULL);

		gl::BindTexture(gl::TEXTURE_2D, 0);
	}
}


void Root::release_and_destroy()
{
	if (m_pixmap != None) {

		gl::BindTexture(gl::TEXTURE_2D, m_texture);
		GLX::ReleaseTexImageEXT(m_display, m_glx_pixmap, GLX_FRONT_EXT);
		gl::BindTexture(gl::TEXTURE_2D, 0);

		m_glx_pixmap = GLX::Pixmap();

		m_pixmap = X11::Pixmap();
	}
}

