#ifndef ORTLE_INPUT_OUTPUT_WINDOW_HPP
#define ORTLE_INPUT_OUTPUT_WINDOW_HPP


#include "managed_window.hpp"

#include "glx/pixmap.hpp"

#include "opengl/core330.hpp"
#include "opengl/texture.hpp"

#include "x11/rectangle_list.hpp"
#include "x11/pixmap.hpp"

#include <X11/Xlib.h>
#include <X11/extensions/shape.h>
// #include <X11/extensions/Xdamage.h>

#include <GL/glx.h>




class FramebufferCache;
class Renderer;


class InputOutputWindow : public ManagedWindow {

public:

	InputOutputWindow(Display* display, Window root, XCreateWindowEvent const& event, XWindowAttributes const& attributes, FramebufferCache& framebuffers);

	InputOutputWindow(InputOutputWindow&& other);
	InputOutputWindow& operator=(InputOutputWindow&& other);

	~InputOutputWindow();

	friend void swap(InputOutputWindow& first, InputOutputWindow& second);


private:

	bool visible_impl() const
	{
		return m_mapped;
	}


private:

	void render_impl(Renderer& renderer);


private:

	void on_configure_notify_impl(XConfigureEvent const& event);
	// void on_damage_notify_impl(XDamageNotifyEvent const&) {}
	void on_graphics_expose_impl(XGraphicsExposeEvent const&) {}
	void on_map_notify_impl(XMapEvent const&);
	void on_no_expose_impl(XNoExposeEvent const&) {}
	void on_property_notify_impl(XPropertyEvent const&) {}
	void on_shape_notify_impl(XShapeEvent const& event);
	void on_unmap_notify_impl(XUnmapEvent const&);


private:

	void reconfigure(int x, int y, int width, int height, int border_width);

	void bind_composite_pixmap();
	void release_composite_pixmap();

	void create_and_bind();
	void release_and_destroy();

	void update_shape_rectangles();


private:

	Display* m_display;
	Window m_root;

	GLXFBConfig m_framebuffer;

	X11::Pixmap m_pixmap;
	GLX::Pixmap m_glx_pixmap;
	OpenGL::Texture m_texture;

	X11::RectangleList m_rectangles;

	int m_x;
	int m_y;
	int m_width;
	int m_height;
	int m_border_width;

	bool m_rgba;
	bool m_shaped;
	bool m_mapped;
	bool m_texture_invalidated;
	// bool m_rectangles_invalidated;

};


#endif
