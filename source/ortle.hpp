#ifndef ORTLE_ORTLE_HPP
#define ORTLE_ORTLE_HPP


#include "framebuffer_cache.hpp"
#include "output_window.hpp"
#include "renderer.hpp"
#include "window_manager.hpp"

#include "x11/composite_manager_atom.hpp"
#include "x11/composite_overlay.hpp"
#include "x11/display.hpp"
#include "x11/error_handler.hpp"
#include "x11/extension.hpp"

#include <X11/Xlib.h>
#include <X11/extensions/shape.h>
// #include <X11/extensions/Xdamage.h>




class Ortle {

public:

	Ortle(int argc, char** argv);

	Ortle(Ortle&&) = delete;
	Ortle& operator=(Ortle&&) = delete;

	~Ortle();


public:

	void run();


private:

	void process_pending_events();

	void on_circulate_notify(XCirculateEvent const& event);
	void on_configure_notify(XConfigureEvent const& event);
	void on_create_notify(XCreateWindowEvent const& event);
	// void on_damage_notify(XDamageNotifyEvent const& event);
	void on_destroy_notify(XDestroyWindowEvent const& event);
	// void on_expose(XExposeEvent const& event);
	void on_graphics_expose(XGraphicsExposeEvent const& event);
	void on_map_notify(XMapEvent const& event);
	void on_no_expose(XNoExposeEvent const& event);
	void on_property_notify(XPropertyEvent const& event);
	void on_reparent_notify(XReparentEvent const& event);
	void on_shape_notify(XShapeEvent const& event);
	void on_unmap_notify(XUnmapEvent const& event);


private:

	X11::ErrorHandler m_x11_error_handler;

	X11::Display m_display;
	int m_screen;
	Window m_root;

	X11::Extension m_composite;
	// X11::Extension m_damage;
	X11::Extension m_fixes;
	X11::Extension m_shape;
	X11::Extension m_glx;

	FramebufferCache m_framebuffers;

	X11::CompositeOverlay m_composite_overlay;

	OutputWindow m_output_window;

	X11::CompositeManagerAtom m_composite_manager_atom;

	Renderer m_renderer;

	WindowManager m_window_manager;

};


#endif
