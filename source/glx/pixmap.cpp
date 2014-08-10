#include "pixmap.hpp"

#include "exceptions.hpp"

#include "../opengl/core330.hpp"

#include <X11/Xlib.h>

#include <GL/glx.h>

#include <cassert>

#include <utility>




namespace GLX {


int const Pixmap::rgb_attributes[] = {

	GLX_TEXTURE_TARGET_EXT, GLX_TEXTURE_2D_EXT,
	GLX_TEXTURE_FORMAT_EXT, GLX_TEXTURE_FORMAT_RGB_EXT,

	None
};


int const Pixmap::rgba_attributes[] = {

	GLX_TEXTURE_TARGET_EXT, GLX_TEXTURE_2D_EXT,
	GLX_TEXTURE_FORMAT_EXT, GLX_TEXTURE_FORMAT_RGBA_EXT,

	None
};




Pixmap::Pixmap()
	: m_display(nullptr)
	, m_glx_pixmap(None)
{}


Pixmap::Pixmap(::Display* display, ::GLXFBConfig framebuffer, ::Pixmap pixmap, int const* attributes)
	: m_display(display)
	, m_glx_pixmap(None)
{
	assert(display != nullptr);
	assert(framebuffer != nullptr);
	assert(pixmap != None);


	::GLXPixmap glx_pixmap = glXCreatePixmap(display, framebuffer, pixmap, attributes);

	if (!glx_pixmap) {
		throw InitializationError("Could not create a new glX Pixmap.");
	}

	m_glx_pixmap = glx_pixmap;
}




Pixmap::Pixmap(Pixmap&& other)
	: Pixmap()
{
	swap(*this, other);
}


Pixmap& Pixmap::operator=(Pixmap&& other)
{
	swap(*this, other);
	return *this;
}




Pixmap::~Pixmap()
{
	if (m_display != nullptr) {
		glXDestroyPixmap(m_display, m_glx_pixmap);
	}
}




void swap(Pixmap& first, Pixmap& second)
{
	using std::swap;

	swap(first.m_display, second.m_display);
	swap(first.m_glx_pixmap, second.m_glx_pixmap);
}


} // namespace GLX

