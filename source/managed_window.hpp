#ifndef ORTLE_MANAGED_WINDOW_HPP
#define ORTLE_MANAGED_WINDOW_HPP


#include "renderer.hpp"

#include <X11/Xlib.h>
#include <X11/extensions/shape.h>
// #include <X11/extensions/Xdamage.h>




class ManagedWindow {

public:

	explicit ManagedWindow(Window window);

	ManagedWindow(ManagedWindow&& other);
	ManagedWindow& operator=(ManagedWindow&& other);

	virtual ~ManagedWindow() {}

	friend void swap(ManagedWindow& first, ManagedWindow& second);


public:

	operator Window() const
	{
		return m_window;
	}


public:

	bool visible() const
	{
		return visible_impl();
	}


public:

	void render(Renderer& renderer)
	{
		render_impl(renderer);
	}


public:

	void on_configure_notify(XConfigureEvent const& event)
	{
		on_configure_notify_impl(event);
	}


	// void on_damage_notify(XDamageNotifyEvent const& event)
	// {
	// 	on_damage_notify_impl(event);
	// }


	void on_graphics_expose(XGraphicsExposeEvent const& event)
	{
		on_graphics_expose_impl(event);
	}


	void on_map_notify(XMapEvent const& event)
	{
		on_map_notify_impl(event);
	}


	void on_no_expose(XNoExposeEvent const& event)
	{
		on_no_expose_impl(event);
	}


	void on_property_notify(XPropertyEvent const& event)
	{
		on_property_notify_impl(event);
	}


	void on_shape_notify(XShapeEvent const& event)
	{
		on_shape_notify_impl(event);
	}


	void on_unmap_notify(XUnmapEvent const& event)
	{
		on_unmap_notify_impl(event);
	}


private:

	virtual bool visible_impl() const = 0;

	virtual void render_impl(Renderer& renderer) = 0;

	virtual void on_configure_notify_impl(XConfigureEvent const& event) = 0;
	// virtual void on_damage_notify_impl(XDamageNotifyEvent const& event) = 0;
	virtual void on_graphics_expose_impl(XGraphicsExposeEvent const& event) = 0;
	virtual void on_map_notify_impl(XMapEvent const& event) = 0;
	virtual void on_no_expose_impl(XNoExposeEvent const& event) = 0;
	virtual void on_property_notify_impl(XPropertyEvent const& event) = 0;
	virtual void on_shape_notify_impl(XShapeEvent const& event) = 0;
	virtual void on_unmap_notify_impl(XUnmapEvent const& event) = 0;


private:

	Window m_window;

};


#endif

