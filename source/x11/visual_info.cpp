#include "visual_info.hpp"

#include "exceptions.hpp"

#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include <GL/glx.h>

#include <cassert>

#include <utility>




namespace X11 {


VisualInfo::VisualInfo(::Display* display, long mask, ::XVisualInfo& vi_template)
	: m_visual_info(nullptr)
{
	assert(display != nullptr);


	int count = 0;
	::XVisualInfo* visual_infos = XGetVisualInfo(display, mask, &vi_template, &count);

	if (!visual_infos) {
		throw InitializationError("Failed to find an XVisualInfo that satisfies the requested template.");
	}

	m_visual_info = visual_infos;
}


VisualInfo::VisualInfo(::Display* display, ::GLXFBConfig framebuffer)
	: m_visual_info(nullptr)
{
	assert(display != nullptr);
	assert(framebuffer != nullptr);


	::XVisualInfo* visual_info = glXGetVisualFromFBConfig(display, framebuffer);

	if (!visual_info) {
		throw InitializationError("Failed to find an XVisualInfo for the requested framebuffer.");
	}

	m_visual_info = visual_info;
}




VisualInfo::VisualInfo(VisualInfo&& other)
	: m_visual_info(nullptr)
{
	swap(*this, other);
}


VisualInfo& VisualInfo::operator=(VisualInfo&& other)
{
	swap(*this, other);
	return *this;
}




VisualInfo::~VisualInfo()
{
	if (m_visual_info != nullptr) {

		XFree(m_visual_info);
	}
}




void swap(VisualInfo& first, VisualInfo& second)
{
	using std::swap;

	swap(first.m_visual_info, second.m_visual_info);
}


} // namespace X11

