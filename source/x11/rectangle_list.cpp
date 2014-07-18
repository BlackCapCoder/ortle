#include "rectangle_list.hpp"

#include "exceptions.hpp"

#include <X11/Xlib.h>
#include <X11/extensions/shape.h>

#include <cassert>

#include <utility>




namespace X11 {


RectangleList::RectangleList()
	: m_display(nullptr)
	, m_window(None)
	, m_count(0)
	, m_rectangles(nullptr)
{}


RectangleList::RectangleList(::Display* display, ::Window window)
	: m_display(display)
	, m_window(window)
	, m_count(0)
	, m_rectangles(nullptr)
{
	assert(display != nullptr);
	assert(window != None);


	int count;
	int ordering;

	::XRectangle* rectangles = XShapeGetRectangles(display, window, ShapeBounding, &count, &ordering);

	if (!rectangles) {
		throw InitializationError("Failed to get list of bounding rectangles.");
	}

	m_count = count;
	m_rectangles = rectangles;
}




RectangleList::RectangleList(RectangleList&& other)
	: RectangleList()
{
	swap(*this, other);
}


RectangleList& RectangleList::operator=(RectangleList&& other)
{
	swap(*this, other);
	return *this;
}




RectangleList::~RectangleList()
{
	if (m_display != nullptr) {
		XFree(m_rectangles);
	}
}




void swap(RectangleList& first, RectangleList& second)
{
	using std::swap;

	swap(first.m_display, second.m_display);
	swap(first.m_window, second.m_window);
	swap(first.m_count, second.m_count);
	swap(first.m_rectangles, second.m_rectangles);
}


} // namespace X11

