#include "framebuffer_cache.hpp"

#include "exceptions.hpp"

#include "opengl/core330.hpp"

#include "utility/trace.hpp"

#include "x11/visual_info.hpp"

#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include <GL/glx.h>

#include <cassert>

#include <map>
#include <utility>




FramebufferCache::FramebufferCache(Display* display, int screen)
	: m_display(display)
	, m_screen(screen)
	, m_framebuffers(nullptr)
	, m_count(0)
	, m_table()
{
	assert(display != nullptr);
	assert(screen >= 0);


	TRACE("loading glx framebuffer configurations");

	int count = 0;
	GLXFBConfig* framebuffers = glXGetFBConfigs(display, screen, &count);

	if (!framebuffers) {
		throw InitializationError("Could not query available framebuffers.");
	}

	m_framebuffers = framebuffers;
	m_count = count;
}




FramebufferCache::FramebufferCache(FramebufferCache&& other)
	: m_display(nullptr)
	, m_screen(0)
	, m_framebuffers(nullptr)
	, m_count(0)
	, m_table()
{
	swap(*this, other);
}


FramebufferCache& FramebufferCache::operator=(FramebufferCache&& other)
{
	swap(*this, other);
	return *this;
}




FramebufferCache::~FramebufferCache()
{
	if (m_framebuffers != nullptr) {
		XFree(m_framebuffers);
	}
}




void swap(FramebufferCache& first, FramebufferCache& second)
{
	using std::swap;

	swap(first.m_display, second.m_display);
	swap(first.m_screen, second.m_screen);
	swap(first.m_framebuffers, second.m_framebuffers);
	swap(first.m_count, second.m_count);
	swap(first.m_table, second.m_table);
}




GLXFBConfig FramebufferCache::choose(int const* attributes)
{
	// this requests a new list rather than use our cached one because it is
	// easier to use glXChooseFBConfig than to duplicate whatever algorithm it
	// uses.  any performance hit is probably negligible.

	GLXFBConfig result = nullptr;

	int count;
	GLXFBConfig* framebuffers = glXChooseFBConfig(m_display, m_screen, attributes, &count);

	if (framebuffers != nullptr) {

		for (int i = 0; i < count; ++i) {

			try {

				X11::VisualInfo info(m_display, framebuffers[i]);

				// add this framebuffer to the table if it's not there already

				if (m_table.find(info->visualid) == m_table.end()) {
					m_table.emplace(info->visualid, framebuffers[i]);
				}

				TRACE("choosing framebuffer with visual", info->visualid);

				result = framebuffers[i];
				break;
			}

			catch (...) {

				// X11::VisualInfo's constructor might fail.  if it does we 
				// just skip the current GLXFBConfig and move on to the next
				// one.
			}
		}

		XFree(framebuffers);
	}

	if (result) {
		return result;
	}
	else {
		throw FramebufferError("Compatible framebuffer could not be chosen.");
	}
}


GLXFBConfig FramebufferCache::find(VisualID visual_id, int depth)
{
	GLXFBConfig result = nullptr;

	// check if we already know this visual id

	auto framebuffer = m_table.find(visual_id);

	if (framebuffer != m_table.end()) {
		result = framebuffer->second;
	}

	else {

		for (int i = 0; i < m_count; ++i) {

			try {

				X11::VisualInfo info(m_display, m_framebuffers[i]);

				int bind_targets = 0;
				int rgb = False;
				int rgba = False;
				int total_depth = 0;
				int alpha_depth = 0;


				glXGetFBConfigAttrib(m_display, m_framebuffers[i], GLX_BIND_TO_TEXTURE_TARGETS_EXT, &bind_targets);
				glXGetFBConfigAttrib(m_display, m_framebuffers[i], GLX_BIND_TO_TEXTURE_RGB_EXT, &rgb);
				glXGetFBConfigAttrib(m_display, m_framebuffers[i], GLX_BIND_TO_TEXTURE_RGBA_EXT, &rgba);
				glXGetFBConfigAttrib(m_display, m_framebuffers[i], GLX_BUFFER_SIZE, &total_depth);
				glXGetFBConfigAttrib(m_display, m_framebuffers[i], GLX_ALPHA_SIZE, &alpha_depth);

				// first check: can we bind this to an rgb/rgba 2d texture?

				if ((bind_targets & GLX_TEXTURE_2D_BIT_EXT) && (rgb == True || rgba == True)) {

					// so at this point we'd like to think that we found an
					// appropriate GLXFBConfig for the requested visual_id and
					// depth.

					// unfortunately the visual associated with a given
					// GLXFBConfig can have mismatched depths.  e.g. on my
					// machine visual 35 has a 32-bit depth, but the associated
					// GLXFBConfig has only 24-bit depth (i.e. no alpha
					// channel).

					// the reverse can also happen.  visual 121 has a 24-bit
					// depth, but the associated GLXFBConfig has a 32-bit
					// depth.  you'd think that would be fine (and it reports
					// that it can bind to an RGBA texture), but in practice it
					// doesn't work.

					// what i do - and i'm not sure this is correct - is look
					// for a GLXFBConfig whose visual has the requested depth,
					// and framebuffer also has that depth, and then (assuming
					// the requested depth is >= 32) prioritize any GLXFBConfig
					// that can bind to an rgba pixmap.

					// note that while it may not be _correct_, ~6 months of use
					// suggests that it is safe enough.

					if (depth == info->depth && depth == total_depth) {

						if (depth >= 32 && rgba) {
							result = m_framebuffers[i];
							break;
						}

						else if (result == nullptr) {
							result = m_framebuffers[i];
						}
					}
				}
			}

			catch (...) {

				// X11::VisualInfo's constructor might fail, but it doesn't
				// matter.  we can just skip that GLXFBConfig.
			}
		}

		if (result) {
			m_table.emplace(visual_id, result);
		}
	}

	if (result) {
		return result;
	}
	else {
		TRACE(visual_id, depth);
		throw FramebufferError("Compatible framebuffer could not be found.");
	}
}

