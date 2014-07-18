#ifndef ORTLE_ROOT_HPP
#define ORTLE_ROOT_HPP


#include "managed_window.hpp"

#include "glx/pixmap.hpp"

#include "opengl/core330.hpp"
#include "opengl/texture.hpp"

// #include "x11/damage.hpp"
#include "x11/pixmap.hpp"
#include "x11/wallpaper_pixmap.hpp"

#include <X11/Xlib.h>
#include <X11/extensions/shape.h>
// #include <X11/extensions/Xdamage.h>

#include <GL/glx.h>




class FramebufferCache;
class Renderer;


class Root : public ManagedWindow {

public:

	Root(Display* display, int screen, Window root, FramebufferCache& framebuffers);

	Root(Root&& other);
	Root& operator=(Root&& other);

	~Root();

	friend void swap(Root& first, Root& second);


private:

	bool visible_impl() const
	{
		return true;
	}


private:

	void render_impl(Renderer& renderer);


private:

	void on_configure_notify_impl(XConfigureEvent const& event);
	// void on_damage_notify_impl(XDamageNotifyEvent const& event);
	void on_graphics_expose_impl(XGraphicsExposeEvent const& event);
	void on_map_notify_impl(XMapEvent const&) {}
	void on_no_expose_impl(XNoExposeEvent const& event);
	void on_property_notify_impl(XPropertyEvent const& event);
	void on_shape_notify_impl(XShapeEvent const&) {}
	void on_unmap_notify_impl(XUnmapEvent const&) {}


private:

	void reconfigure(int width, int height, int border_width);

	void bind_composite_pixmap();
	void release_composite_pixmap();

	void create_and_bind();
	void release_and_destroy();


private:

	Display* m_display;
	int m_screen;
	Window m_root;

	GLXFBConfig m_framebuffer;

	// X11::Damage m_damage;

	X11::Pixmap m_pixmap;
	GLX::Pixmap m_glx_pixmap;
	OpenGL::Texture m_texture;

	int m_width;
	int m_height;

	bool m_rgba;
	bool m_waiting_for_success;

};


#endif
