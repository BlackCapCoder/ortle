#ifndef ORTLE_INPUT_ONLY_WINDOW_HPP
#define ORTLE_INPUT_ONLY_WINDOW_HPP


#include "managed_window.hpp"
#include "renderer.hpp"

#include <X11/Xlib.h>
#include <X11/extensions/shape.h>
// #include <X11/extensions/Xdamage.h>




class InputOnlyWindow : public ManagedWindow {

public:

	explicit InputOnlyWindow(XCreateWindowEvent const& event);

	InputOnlyWindow(InputOnlyWindow&& other);
	InputOnlyWindow& operator=(InputOnlyWindow&& other);

	~InputOnlyWindow();

	friend void swap(InputOnlyWindow& first, InputOnlyWindow& second);


private:

	bool visible_impl() const { return false; }

	void render_impl(Renderer&) {}

	void on_configure_notify_impl(XConfigureEvent const&) {}
	// void on_damage_notify_impl(XDamageNotifyEvent const&) {}
	void on_graphics_expose_impl(XGraphicsExposeEvent const&) {}
	void on_map_notify_impl(XMapEvent const&) {}
	void on_no_expose_impl(XNoExposeEvent const&) {}
	void on_property_notify_impl(XPropertyEvent const&) {}
	void on_shape_notify_impl(XShapeEvent const&) {}
	void on_unmap_notify_impl(XUnmapEvent const&) {}

};


#endif

