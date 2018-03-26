#include "window_manager.hpp"

#include "exceptions.hpp"
#include "framebuffer_cache.hpp"
#include "input_only_window.hpp"
#include "input_output_window.hpp"
#include "managed_window.hpp"
#include "root.hpp"

#include "utility/trace.hpp"

#include "x11/functions.hpp"

#include <X11/Xlib.h>
#include <X11/extensions/shape.h>
#include <X11/extensions/Xcomposite.h>
#include <X11/extensions/Xdamage.h>

#include <cassert>

#include <algorithm>
#include <memory>
#include <utility>
#include <vector>




namespace {


template<typename Iterator>
inline Iterator find(Iterator begin, Iterator end, Window window)
{
	return std::find_if(begin, end, [=](std::unique_ptr<ManagedWindow> const& w) { return *w == window; });
}


template<typename Container, typename Iterator>
inline void move_before(Container& container, Iterator item, Iterator target)
{
	if (item == target) {
		return;
	}

	auto temp = std::move(*item);

	if (item < target) {
		auto it = std::move(item + 1, target, item);
		*it = std::move(temp);
	}

	else {
		auto it = std::move_backward(target, item, item + 1);
		*(--it) = std::move(temp);
	}
}


} // namespace




WindowManager::WindowManager(Display* display, int screen, Window root, FramebufferCache& framebuffers)
	: m_display(display)
	, m_screen(0)
	, m_root(root)
	, m_windows()
{
	assert(display != nullptr);
	assert(screen >= 0);
	assert(root != None);


	TRACE("creating window manager on root", root);

	XGrabServer(display);

	m_windows.emplace(m_windows.begin(), new Root(display, screen, root, framebuffers));

	XCompositeRedirectSubwindows(m_display, m_root, CompositeRedirectManual);

	XSelectInput(m_display, m_root, PropertyChangeMask | StructureNotifyMask | SubstructureNotifyMask);


	Window tree_root;
	Window tree_parent;
	Window* tree_children;
	unsigned int count;

	if (XQueryTree(display, root, &tree_root, &tree_parent, &tree_children, &count)) {

		assert(root == tree_root);

		for (unsigned int i = 0; i < count; ++i) {
			XCreateWindowEvent fake_event;
			fake_event.type = -1;
			fake_event.parent = root;
			fake_event.window = tree_children[i];
			add_before(m_windows.end(), fake_event, framebuffers);

		}
		XFree(tree_children);
	}

	XUngrabServer(display);
}




WindowManager::WindowManager(WindowManager&& other)
	: m_display(nullptr)
	, m_screen(0)
	, m_root(None)
	, m_windows()
{
	swap(*this, other);
}


WindowManager& WindowManager::operator=(WindowManager&& other)
{
	swap(*this, other);
	return *this;
}




WindowManager::~WindowManager()
{
	if (m_display != nullptr) {

		TRACE("destroying window manager on root", m_root);

		XCompositeUnredirectSubwindows(m_display, m_root, CompositeRedirectManual);
	}
}




void swap(WindowManager& first, WindowManager& second)
{
	using std::swap;

	swap(first.m_display, second.m_display);
	swap(first.m_screen, second.m_screen);
	swap(first.m_root, second.m_root);
	swap(first.m_windows, second.m_windows);
}




void WindowManager::add_before(Iterator target, XCreateWindowEvent const& event, FramebufferCache& framebuffers)
{
	assert(event.window != None);
	assert(event.parent != None);


	TRACE("starting management of window", event.window);

	XWindowAttributes attributes;
	if (!XGetWindowAttributes(m_display, event.window, &attributes)) {
		// this window is about to be destroyed.  treat it as an inputonly
		// window so the renderer ignores it, and we can still use it for
		// stacking
		attributes.c_class = InputOnly;
	}

	if (find(m_windows.begin(), m_windows.end(), event.window) != m_windows.end()) {
		// we are already managing this window.  this should not happen.
		TRACE("duplicate manage request for window", event.window);
		return;
	}


	if (attributes.c_class == InputOutput) {
		XShapeSelectInput(m_display, event.window, ShapeNotifyMask);
		m_windows.emplace(target, new InputOutputWindow(m_display, m_root, event, attributes, framebuffers));
	}
	else {
		m_windows.emplace(target, new InputOnlyWindow(event));
	}
}


void WindowManager::move_before(Iterator target, Iterator window)
{
	if (window == target) {
		return;
	}

	auto temp = std::move(*window);

	if (window < target) {
		auto it = std::move(window + 1, target, window);
		*it = std::move(temp);
	}

	else {
		auto it = std::move_backward(target, window, window + 1);
		*(--it) = std::move(temp);
	}
}


void WindowManager::remove(Iterator target)
{
	TRACE("stopping management of window", **target);

	m_windows.erase(target);
}




void WindowManager::on_circulate_notify(XCirculateEvent const& event)
{
	auto begin = m_windows.begin();
	auto end = m_windows.end();

	auto window = find(begin, end, event.window);

	if (window != end) {
		if (event.place == PlaceOnTop) {
			move_before(end, window);
		}
		else {
			move_before(++begin, window);
		}
	}
	else {
		TRACE("WARNING", "XCirculateEvent.window missing from stack", event.window);
	}
}


void WindowManager::on_configure_notify(XConfigureEvent const& event)
{
	auto begin = m_windows.begin();
	auto end = m_windows.end();

	auto window = find(begin, end, event.window);

	if (window != end) {

		// any adjustment to the stack may invalidate these iterators, so we 
		// pass along the configure event before adjusting the stack

		(*window)->on_configure_notify(event);

		
		// now we try to restack the window if it is necessary

		auto above = end;

		// case 1: event.above is not None.  this means that event.window was 
		// stacked above event.above.  try to find that window in our stack.

		if (event.above != None) {
			above = find(begin, end, event.above);
		}

		// case 2: event.above is None, and the window has been stacked below 
		// all of its siblings.  this means it has been stacked directly above 
		// the root window.

		else {
			above = begin;
		}


		// now that we know where to put it, try to move the window to its new 
		// location.  this can invalidate any of the iterators here.

		if (above != end) {
			move_before(++above, window);
		}

		else {
			TRACE("WARNING", "XConfigureEvent.above missing from the stack", event.above);
		}
	}
}


void WindowManager::on_create_notify(XCreateWindowEvent const& event, FramebufferCache& framebuffers)
{
	// only consider windows parented to the root window.  this check should 
	// be unnecessary, but i've received some errant UnmapNotifies from windows 
	// that are not my business, so maybe i'll get some here, too.

	if (event.parent == m_root) {
		add_before(m_windows.end(), event, framebuffers);
	}
	else {
		TRACE("WARNING", "XCreateWindowEvent.parent is not the root window", "event.window", event.window, "event.parent", event.parent);
	}
}


// void WindowManager::on_damage_notify(XDamageNotifyEvent const& event)
// {
// 	auto begin = m_windows.begin();
// 	auto end = m_windows.end();

// 	auto window = find(begin, end, event.drawable);

// 	if (window != end) {
// 		(*window)->on_damage_notify(event);
// 	}
// 	else {
// 		TRACE("WARNING", "XDamageNotifyEvent.drawable missing from stack", event.drawable);
// 	}
// }


void WindowManager::on_destroy_notify(XDestroyWindowEvent const& event)
{
	auto begin = m_windows.begin();
	auto end = m_windows.end();

	auto window = find(begin, end, event.window);

	if (window != end) {
		remove(window);
	}
	else {
		TRACE("WARNING", "XDestroyWindowEvent.window missing from stack", event.window);
	}
}


void WindowManager::on_graphics_expose(XGraphicsExposeEvent const& event)
{
	assert(m_windows.begin() != m_windows.end());

	(*m_windows.begin())->on_graphics_expose(event);
}


void WindowManager::on_map_notify(XMapEvent const& event)
{
	auto begin = m_windows.begin();
	auto end = m_windows.end();

	auto window = find(begin, end, event.window);

	if (window != end) {
		(*window)->on_map_notify(event);
	}
	else {
		TRACE("WARNING", "XMapEvent.window missing from stack", event.window);
	}
}


void WindowManager::on_no_expose(XNoExposeEvent const& event)
{
	assert(m_windows.begin() != m_windows.end());

	(*m_windows.begin())->on_no_expose(event);
}


void WindowManager::on_property_notify(XPropertyEvent const& event)
{
	auto begin = m_windows.begin();
	auto end = m_windows.end();

	auto window = find(begin, end, event.window);

	if (window != end) {
		(*window)->on_property_notify(event);
	}
	else {
		TRACE("WARNING", "XPropertyEvent.window missing from stack", event.window);
	}
}


void WindowManager::on_reparent_notify(XReparentEvent const& event, FramebufferCache& framebuffers)
{
	// we are interested in two cases when a window is reparented.  first, when
	// the window has been reparented to the root window, and we're not 
	// managing it; and second, when a window has been reparented to anything 
	// other than the root window, and we _are_ managing it.

	// the reason why was not obvious.  it turns out that for ortle to work, 
	// we only need to keep track of windows parented to the root window.  i'm 
	// not sure why, but it turns out that XCompositeNameWindowPixmap includes 
	// child windows in its image, which means that we don't need to bother 
	// with child windows at all.


	auto begin = m_windows.begin();
	auto end = m_windows.end();

	auto window = find(begin, end, event.window);

	// case 1: event.parent is the root window.  look for the window in our 
	// stack.  if we don't find it, add it.

	if (event.parent == m_root) {

		if (window == end) {

			XCreateWindowEvent fake_event;
			fake_event.type = ReparentNotify;
			fake_event.parent = event.parent;
			fake_event.window = event.window;

			add_before(end, fake_event, framebuffers);
		}
	}

	// case 2: event.parent is _not_ the root window.  look for the window in 
	// our stack.  if we find it, remove it.  note that we stop watching shape
	// input here rather than in remove() because remove() may be called after
	// a window has been destroyed.

	else {
		if (window != end) {
			XShapeSelectInput(m_display, **window, NoEventMask);
			remove(window);
		}
	}
}


void WindowManager::on_shape_notify(XShapeEvent const& event)
{
	// logically, this check belongs in input_output_window.cpp, but we save
	// a little effort searching through the list if we do it here.

	if (event.kind != ShapeBounding || event.width < 1 || event.height < 1) {
		return;
	}

	auto begin = m_windows.begin();
	auto end = m_windows.end();

	auto window = find(begin, end, event.window);

	if (window != end) {
		(*window)->on_shape_notify(event);
	}
	else {
		TRACE("WARNING", "XShapeEvent.window missing from stack", event.window);
	}
}


void WindowManager::on_unmap_notify(XUnmapEvent const& event)
{
	auto begin = m_windows.begin();
	auto end = m_windows.end();

	auto window = find(begin, end, event.window);

	if (window != end) {
		(*window)->on_unmap_notify(event);
	}
	else {
		TRACE("WARNING", "XUnmapEvent.window missing from stack", event.window);
	}
}

