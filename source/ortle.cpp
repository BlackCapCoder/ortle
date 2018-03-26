#include "ortle.hpp"

#include "framebuffer_cache.hpp"
#include "output_window.hpp"
#include "renderer.hpp"
#include "window_manager.hpp"

#include "glx/functions.hpp"

#include "opengl/core330.hpp"
#include "opengl/exceptions.hpp"

#include "utility/backtrace.hpp"
#include "utility/trace.hpp"

#include "x11/composite_manager_atom.hpp"
#include "x11/composite_overlay.hpp"
#include "x11/display.hpp"
#include "x11/error_handler.hpp"
#include "x11/extension.hpp"
#include "x11/geometry.hpp"

#include <unistd.h>

#include <X11/Xlib.h>
#include <X11/Xproto.h>
#include <X11/extensions/shape.h>
#include <X11/extensions/shapeproto.h>
#include <X11/extensions/Xcomposite.h>
#include <X11/extensions/Xdamage.h>
#include <X11/extensions/Xfixes.h>

#include <csignal>

#include <iostream>

#ifdef GHETTO_PROFILE
	#include <chrono>
#endif





namespace {


volatile std::sig_atomic_t g_running = 1;


void signal_handler(int)
{
	g_running = 0;
}


int x11_error_handler(Display*, XErrorEvent* error)
{
	// i try as much as possible to rely only upon the events the X server
	// sends me to maintain the local copy of the server's state, but
	// sometimes the events are not sufficient, and i need to query the
	// server.  these requests generally use information that is a little
	// behind the server's current state, and can therefore generate errors.

	// this also means that the information returned is a little ahead of the
	// local copy, but since these requests are sent in the middle of the event
	// loop (or when the server is grabbed), the local state _should_ catch up
	// to them before any drawing is done.


	// XGetWindowAttributes is used to determine the class (e.g. InputOnly)
	// and dimensions of a window when adding it to the window manager.  it
	// can generate a BadWindow event if the window has already been
	// destroyed.

	if (error->request_code == X_GetWindowAttributes && error->error_code == BadWindow) {
		TRACE("WARNING", "XGetWindowAttributes generated a BadWindow error");
		return 0;
	}


	// XCompositeNameWindowPixmap is used to get a pixmap of a given window.
	// it can similarly fail if the window has already been destroyed, but can
	// also fail if the window is off-screen or otherwise invisible.  in such
	// cases, its return value of None is used to determine that the window
	// should not be drawn.

	else if (error->request_code == X_CompositeNameWindowPixmap) {
		TRACE("WARNING", "XCompositeNameWindowPixmap failed", error->error_code);
		return 0;
	}


	// XShapeGetRectangles fails for mysterious reasons that are assumed to be
	// similar, if not the same, to the other two errors above.

	else if (error->request_code == X_ShapeGetRectangles) {
		TRACE("WARNING", "XShapeGetRectangles failed", error->error_code);
		return 0;
	}


	// XCopyArea is used when we try to copy the root window's wallpaper.
	// since this is set as a property on the root window, and we have no idea
	// if it's even a pixmap, let alone one of the right size, depth, etc. we
	// need to catch the error here.

	// since we wait on an XNoExposeEvent before attempting to draw the result
	// of a copy, eating this event should be safe.

	else if (error->request_code == X_CopyArea && (error->error_code == BadDrawable || error->error_code == BadMatch)) {
		TRACE("WARNING", "XCopyArea failed", error->error_code);
		return 0;
	}


	// all other error codes are presumably bugs that i need to fix.

#ifdef DEBUG_SYNCHRONIZE

	Utility::Backtrace(STDERR_FILENO);

#endif

	return error->error_code;
}




// pending_shape_notify
// predicate used with XCheckIfEvent to compress shape events for a window, so
// that only the most recent event is processed.

Bool pending_shape_notify(Display*, XEvent* event, XPointer arg)
{
	XShapeEvent* shape_event = reinterpret_cast<XShapeEvent*>(arg);
	if (event->type == shape_event->type && reinterpret_cast<XShapeEvent*>(event)->window == shape_event->window) {
		return True;
	}
	return False;
}


} // namespace




Ortle::Ortle(int, char**)

	: m_x11_error_handler(x11_error_handler)

	, m_display(NULL)
	, m_screen(XDefaultScreen(m_display))
	, m_root(XRootWindow(m_display, m_screen))

	, m_composite(m_display, "XComposite", 0, 4, &XCompositeQueryExtension, &XCompositeQueryVersion)
	, m_damage(m_display, "XDamage", 1, 1, &XDamageQueryExtension, &XDamageQueryVersion)
	, m_fixes(m_display, "XFixes", 2, 0, &XFixesQueryExtension, &XFixesQueryVersion)
	, m_shape(m_display, "XShape", 1, 1, &XShapeQueryExtension, &XShapeQueryVersion)
	, m_glx(m_display, "GLX", 1, 4, &glXQueryExtension, &glXQueryVersion)

	, m_framebuffers(m_display, m_screen)

	, m_composite_overlay(m_display, m_root)

	, m_output_window(m_display, m_root, m_composite_overlay, m_framebuffers)

	, m_composite_manager_atom(m_display, m_screen, m_output_window)

	, m_renderer()

	, m_window_manager(m_display, m_screen, m_root, m_framebuffers)

{
	std::signal(SIGHUP, signal_handler);
	std::signal(SIGINT, signal_handler);
	std::signal(SIGTERM, signal_handler);


#ifdef DEBUG_SYNCHRONIZE

	XSynchronize(m_display, True);

#endif

}


Ortle::~Ortle()
{
	// nothing to do
}




#ifdef GHETTO_PROFILE

	void Ortle::run()
	{
		unsigned int last_retrace = 0;

		m_output_window.reconfigure();
		m_output_window.swap_interval(1);

		X11::Geometry root_geometry(m_display, m_root);
		m_renderer.set_viewport(root_geometry.width, root_geometry.height);

		gl::ClearColor(0.0f, 0.0f, 0.0f, 1.0f);

		auto start = std::chrono::high_resolution_clock::now();

		auto event_ticks = start - start;
		auto render_ticks = event_ticks;
		auto swap_ticks = event_ticks;
		auto wait_ticks = event_ticks;

		int iteration = 0;

		while (g_running) {

			unsigned int current_retrace = 0;

			++iteration;
			iteration %= 60;

			if (iteration == 0) {
				TRACE("event ticks", event_ticks.count());
				TRACE("render ticks", render_ticks.count());
				TRACE("swap ticks", swap_ticks.count());
				TRACE("wait ticks", wait_ticks.count());
				TRACE("total", event_ticks.count() + render_ticks.count() + swap_ticks.count() + wait_ticks.count());

				event_ticks -= event_ticks;
				render_ticks -= render_ticks;
				swap_ticks -= swap_ticks;
				wait_ticks -= wait_ticks;
			}

			// usually this is done in a while loop because there can be  more than
			// one error waiting.  but, for now at least, every openglerror is
			// fatal.

			GLenum error = gl::NO_ERROR_;
			if ((error = gl::GetError()) != gl::NO_ERROR_) {
				throw OpenGL::StateError("Error in OpenGL State", error);
			}

			auto p0 = std::chrono::high_resolution_clock::now();

			process_pending_events();

			// XSync(m_display, False);

			auto p1 = std::chrono::high_resolution_clock::now();

			// XSync(m_display, False);
			// glXWaitX();
			// gl::Flush();

			gl::Clear(gl::COLOR_BUFFER_BIT);

			m_renderer.render(m_window_manager.begin(), m_window_manager.end());


			// a current problem is that everything lags when moving a window over
			// an accelerated chromium window.  what happens is that we spend too
			// much time in glXSwapBuffers.

			// unsigned int current_retrace = 0;
			// GLX::GetVideoSyncSGI(&current_retrace);

			// option one: wait for the next retrace:
			// this is no good because you can miss it and end up waiting a long
			// time

			// GLX::WaitVideoSyncSGI(current_retrace, 1, &current_retrace);


			// option two: wait for the next odd frame if current is even, or wait
			// for the next even frame is current is odd

			// this is what compton does and might be as good as we can get.  it
			// is still bad because the skipping frame is ugly.

			// GLX::WaitVideoSyncSGI(2, (current_retrace + 1) % 2, &current_retrace);


			auto p2 = std::chrono::high_resolution_clock::now();

			// XSync(m_display, False);
			// glXWaitX();
			// gl::Flush();

			// gl::Finish();

			// TRACE("RETRACE", last_retrace, current_retrace);


			m_output_window.swap_buffers();

			auto p3 = std::chrono::high_resolution_clock::now();

			if (GLX::WaitVideoSyncSGI) {

				GLX::WaitVideoSyncSGI(1, 0, &current_retrace);

				if (current_retrace == last_retrace) {
					TRACE("WARNING", last_retrace, current_retrace);
				}
				else if (current_retrace > last_retrace + 1) {
					TRACE("WARNING", last_retrace, current_retrace);
				}

				last_retrace = current_retrace;
			}

			auto p4 = std::chrono::high_resolution_clock::now();


			event_ticks += p1 - p0;
			render_ticks += p2 - p1;
			swap_ticks += p3 - p2;
			wait_ticks += p4 - p3;

			if ((p4-p0).count() > 1e9 / 45) {
				TRACE("TICKS", "1-0", (p1 - p0).count());
				TRACE("TICKS", "2-1", (p2 - p1).count());
				TRACE("TICKS", "3-2", (p3 - p2).count());
				TRACE("TICKS", "4-3", (p4 - p3).count());
				TRACE("TICKS", "tot", (p3-p0).count());
			}
		}
	}


#else

	void Ortle::run()
	{
		unsigned int last_retrace = 0;

		m_output_window.reconfigure();
		m_output_window.swap_interval(1);

		X11::Geometry root_geometry(m_display, m_root);
		m_renderer.set_viewport(root_geometry.width, root_geometry.height);

		gl::ClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // int itt = 0;
		while (g_running) {
      // if (itt++ % 2 != 0) {
      //   unsigned int current_retrace = 0;
			// 	GLX::WaitVideoSyncSGI(1, 0, &current_retrace);
      //   continue;
      // }

			// usually this is done in a while loop because there can be  more than
			// one error waiting.  but, for now at least, every openglerror is
			// fatal.

			GLenum error = gl::NO_ERROR_;
			if ((error = gl::GetError()) != gl::NO_ERROR_) {
				throw OpenGL::StateError("Error in OpenGL State", error);
			}


			process_pending_events();


			gl::Clear(gl::COLOR_BUFFER_BIT);

			m_renderer.render(m_window_manager.begin(), m_window_manager.end());


			m_output_window.swap_buffers();


			if (GLX::WaitVideoSyncSGI) {

				unsigned int current_retrace = 0;

				GLX::WaitVideoSyncSGI(1, 0, &current_retrace);

				if (current_retrace == last_retrace) {
					TRACE("WARNING", last_retrace, current_retrace);
				}
				else if (current_retrace > last_retrace + 1) {
					TRACE("WARNING", last_retrace, current_retrace);
				}

				last_retrace = current_retrace;
			}
		}
	}

#endif 




void Ortle::process_pending_events()
{
	while (g_running && XPending(m_display) > 0) {

		XEvent event;
		XNextEvent(m_display, &event);

    // printf("%i\n", event.type);

		switch (event.type) {

			case CirculateNotify:
				on_circulate_notify(event.xcirculate);
				break;

			case ClientMessage:
				TRACE("client message", event.xclient.window, event.xclient.message_type, event.xclient.format);
				break;

			case ConfigureNotify:
				on_configure_notify(event.xconfigure);
				break;


			case CreateNotify:
				on_create_notify(event.xcreatewindow);
				break;

			case DestroyNotify:
				on_destroy_notify(event.xdestroywindow);
				break;

			// case Expose:
			// 	on_expose(event.xexpose);
				// break;
      case XDamageNotify:
        printf("xdamage\n");
        break;

			case GraphicsExpose:
				on_graphics_expose(event.xgraphicsexpose);
				break;

			case MapNotify:
				on_map_notify(event.xmap);
				break;

			case NoExpose:
				on_no_expose(event.xnoexpose);
				break;

			case PropertyNotify:
				on_property_notify(event.xproperty);
				break;

			case ReparentNotify:
				on_reparent_notify(event.xreparent);
				break;

			case UnmapNotify:
				on_unmap_notify(event.xunmap);
				break;

			default:

				if (event.type == ShapeNotify + m_shape.event_base) {
					while (XCheckIfEvent(m_display, &event, &pending_shape_notify, reinterpret_cast<XPointer>(&event)) == True) {
						TRACE("pending ShapeNotify event found, ignoring this one.");
					}
					on_shape_notify(reinterpret_cast<XShapeEvent&>(event));
				}

				// else if (event.type == XDamageNotify + m_damage.event_base) {
				// 	on_damage_notify(reinterpret_cast<XDamageNotifyEvent&>(event));
				// }

				else {
					TRACE("WARNING", "unhandled event", event.type);
				}
		}
	}
}




void Ortle::on_circulate_notify(XCirculateEvent const& event)
{
	// raised when event.window is circulated either above or below all of its
	// siblings.

	TRACE(event.window, event.place);

	m_window_manager.on_circulate_notify(event);
}


void Ortle::on_configure_notify(XConfigureEvent const& event)
{
	// raised when event.window is configured, which can include changing
	// its position, size, border width, or stacking order.

	TRACE(event.window, event.x, event.y, event.width, event.height, event.above);

	// if it's the root window, we need to reconfigure our output window and
	// viewport

	if (event.window == m_root) {
		m_output_window.reconfigure();
		m_renderer.set_viewport(event.width, event.height);
	}

	// pass the event along to the window manager

	m_window_manager.on_configure_notify(event);
}


void Ortle::on_create_notify(XCreateWindowEvent const& event)
{
	// raised when event.window is created

	TRACE(event.window, event.parent, event.x, event.y, event.width, event.height, event.override_redirect);

	m_window_manager.on_create_notify(event, m_framebuffers);
}


// void Ortle::on_damage_notify(XDamageNotifyEvent const& event)
// {
// 	TRACE(event.drawable);

// 	if (event.drawable == m_root) {
// 		m_window_manager.on_damage_notify(event);
// 	}
// }


void Ortle::on_destroy_notify(XDestroyWindowEvent const& event)
{
	// raised when event.window is destroyed
	
	TRACE(event.window);

	m_window_manager.on_destroy_notify(event);
}


// void Ortle::on_expose(XExposeEvent const& event)
// {
// 	TRACE(event.window, event.x, event.y, event.width, event.height);
// }


void Ortle::on_graphics_expose(XGraphicsExposeEvent const& event)
{
	// raised when XCopyArea or XCopyPlane fails when the source of the copy
	// is either not available (e.g. an obscured root window), or the rquested
	// area is out of the source's bounds.

	TRACE(event.drawable, event.x, event.y, event.width, event.y);

	m_window_manager.on_graphics_expose(event);
}


void Ortle::on_map_notify(XMapEvent const& event)
{
	// raised when event.window is mapped

	TRACE(event.window, event.override_redirect);

	m_window_manager.on_map_notify(event);
}


void Ortle::on_no_expose(XNoExposeEvent const& event)
{
	// raised when XCopyArea or XCopyPlane works.  this tells us that we
	// succeeded when trying to copy the root window pixmap, and we can now
	// draw th eroot window.

	TRACE(event.drawable);

	m_window_manager.on_no_expose(event);
}

void Ortle::on_property_notify(XPropertyEvent const& event)
{
	// raised when one of event.window's properties changes.  we are only
	// interested when this happens on the root window, and then only when its
	// wallpaper pixmap property changes.

	TRACE(event.window, event.atom);

	if (event.window == m_root) {
		m_window_manager.on_property_notify(event);
	}
}


void Ortle::on_reparent_notify(XReparentEvent const& event)
{
	// raised when event.window gets a new parent

	// this is explained more in window_manager.cpp, but the short version is:
	// we need to start managing event.window if it has been reparented to the
	// root window, and we need to stop managing it if it has been reparented
	// to anything else.

	TRACE(event.window, event.parent, event.x, event.y);

	m_window_manager.on_reparent_notify(event, m_framebuffers);
}


void Ortle::on_shape_notify(XShapeEvent const& event)
{
	// raised when event.window's shape changes

	TRACE(event.window, "shaped", event.shaped, "extents", event.x, event.y, event.width, event.height);

	m_window_manager.on_shape_notify(event);
}


void Ortle::on_unmap_notify(XUnmapEvent const& event)
{
	// raised when event.window is unmapped

	TRACE(event.window);

	m_window_manager.on_unmap_notify(event);
}

