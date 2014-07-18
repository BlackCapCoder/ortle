#include "composite_overlay.hpp"

#include "exceptions.hpp"

#include <X11/Xlib.h>
#include <X11/extensions/Xcomposite.h>

#include <cassert>

#include <utility>




namespace X11 {


CompositeOverlay::CompositeOverlay(::Display* display, ::Window root)
	: m_display(display)
	, m_root(root)
	, m_composite_overlay(None)
{
	assert(display != nullptr);
	assert(root != None);


	::Window composite_overlay = XCompositeGetOverlayWindow(display, root);

	if (!composite_overlay) {
		throw InitializationError("Could not bind composite overlay window.");
	}

	XSelectInput(display, composite_overlay, StructureNotifyMask);

	m_composite_overlay = composite_overlay;
}




CompositeOverlay::CompositeOverlay(CompositeOverlay&& other)
	: m_display(nullptr)
	, m_root(None)
	, m_composite_overlay(None)
{
	swap(*this, other);
}


CompositeOverlay& CompositeOverlay::operator=(CompositeOverlay&& other)
{
	swap(*this, other);
	return *this;
}




CompositeOverlay::~CompositeOverlay()
{
	if (m_display != nullptr) {
		XCompositeReleaseOverlayWindow(m_display, m_root);
	}
}




void swap(CompositeOverlay& first, CompositeOverlay& second)
{
	using std::swap;

	swap(first.m_display, second.m_display);
	swap(first.m_root, second.m_root);
	swap(first.m_composite_overlay, second.m_composite_overlay);
}


} // namespace X11

