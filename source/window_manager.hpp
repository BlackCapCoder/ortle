#ifndef ORTLE_WINDOW_MANAGER_HPP
#define ORTLE_WINDOW_MANAGER_HPP


#include <X11/Xlib.h>
#include <X11/extensions/shape.h>
#include <X11/extensions/Xdamage.h>

#include <memory>
#include <vector>




class FramebufferCache;
class ManagedWindow;


class WindowManager {

public:

	using Container = std::vector<std::unique_ptr<ManagedWindow>>;
	using Iterator = Container::iterator;


public:

	WindowManager(Display* display, int screen, Window root, FramebufferCache& framebuffers);

	WindowManager(WindowManager&& other);
	WindowManager& operator=(WindowManager&& other);

	~WindowManager();

	friend void swap(WindowManager& first, WindowManager& second);


public:

	Iterator begin() {
		return m_windows.begin();
	}


	Iterator end() {
		return m_windows.end();
	}


public:

	void on_circulate_notify(XCirculateEvent const& event);
	void on_configure_notify(XConfigureEvent const& event);
	void on_create_notify(XCreateWindowEvent const& event, FramebufferCache& framebuffers);
	void on_damage_notify(XDamageNotifyEvent const& event);
	void on_destroy_notify(XDestroyWindowEvent const& event);
	void on_graphics_expose(XGraphicsExposeEvent const& event);
	void on_map_notify(XMapEvent const& event);
	void on_no_expose(XNoExposeEvent const& event);
	void on_property_notify(XPropertyEvent const& event);
	void on_reparent_notify(XReparentEvent const& event, FramebufferCache& framebuffers);
	void on_shape_notify(XShapeEvent const& event);
	void on_unmap_notify(XUnmapEvent const& event);


private:

	void add_before(Iterator target, XCreateWindowEvent const& event, FramebufferCache& framebuffers);
	void move_before(Iterator target, Iterator window);
	void remove(Iterator target);


private:

	Display* m_display;
	int m_screen;
	Window m_root;

	Container m_windows;

};



#endif
