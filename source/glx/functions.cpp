#include "functions.hpp"

#include "exceptions.hpp"

#include "../opengl/core330.hpp"

#include <X11/Xlib.h>

#include <GL/glx.h>

#include <cassert>




namespace GLX {


using CreateContextAttribsARB_sig = ::GLXContext (*)(::Display*, ::GLXFBConfig, ::GLXContext, Bool, int const*);
CreateContextAttribsARB_sig CreateContextAttribsARB = nullptr;




using BindTexImageEXT_sig = void (*)(::Display*, ::GLXDrawable, int, int const*);
BindTexImageEXT_sig BindTexImageEXT = nullptr;


using ReleaseTexImageEXT_sig = void (*)(::Display*, ::GLXDrawable, int);
ReleaseTexImageEXT_sig ReleaseTexImageEXT = nullptr;


using SwapIntervalEXT_sig = void (*)(::Display*, ::GLXDrawable, int);
SwapIntervalEXT_sig SwapIntervalEXT = nullptr;


using SwapIntervalMESA_sig = int (*)(int);
SwapIntervalMESA_sig SwapIntervalMESA = nullptr;




using GetVideoSyncSGI_sig = int (*)(unsigned int*);
GetVideoSyncSGI_sig GetVideoSyncSGI = nullptr;


using WaitVideoSyncSGI_sig = int (*)(int, int, unsigned int*);
WaitVideoSyncSGI_sig WaitVideoSyncSGI = nullptr;




void load_functions()
{
	if (CreateContextAttribsARB == nullptr) {
		CreateContextAttribsARB = reinterpret_cast<CreateContextAttribsARB_sig>(glXGetProcAddress(reinterpret_cast<GLubyte const*>("glXCreateContextAttribsARB")));
		if (!CreateContextAttribsARB) {
			throw GLX::InitializationError("Failed to load glXCreateContextAttribsARB.");
		}
	}



	if (BindTexImageEXT == nullptr) {
		BindTexImageEXT = reinterpret_cast<BindTexImageEXT_sig>(glXGetProcAddress(reinterpret_cast<GLubyte const*>("glXBindTexImageEXT")));
		if (!BindTexImageEXT) {
			throw GLX::InitializationError("Failed to load glXBindTexImageEXT.");
		}
	}

	if (ReleaseTexImageEXT == nullptr) {
		ReleaseTexImageEXT = reinterpret_cast<ReleaseTexImageEXT_sig>(glXGetProcAddress(reinterpret_cast<GLubyte const*>("glXReleaseTexImageEXT")));
		if (!ReleaseTexImageEXT) {
			throw GLX::InitializationError("Failed to load glXReleaseTexImageEXT.");
		}
	}

	if (SwapIntervalEXT == nullptr) {
		SwapIntervalEXT = reinterpret_cast<SwapIntervalEXT_sig>(glXGetProcAddress(reinterpret_cast<GLubyte const*>("glXSwapIntervalEXT")));
		if (!SwapIntervalEXT) {
			// throw GLX::InitializationError("Failed to load glXSwapIntervalEXT.");
		}
	}

	if (SwapIntervalMESA == nullptr) {
		SwapIntervalMESA = reinterpret_cast<SwapIntervalMESA_sig>(glXGetProcAddress(reinterpret_cast<GLubyte const*>("glXSwapIntervalMESA")));
		if (!SwapIntervalMESA) {
			// throw GLX::InitializationError("Failed to load glXSwapIntervalMESA.");
		}
	}



	if (GetVideoSyncSGI == nullptr) {
		GetVideoSyncSGI = reinterpret_cast<GetVideoSyncSGI_sig>(glXGetProcAddress(reinterpret_cast<GLubyte const*>("glXGetVideoSyncSGI")));
		if (!GetVideoSyncSGI) {
			// throw GLX::InitializationError("Failed to load glXGetVideoSyncSGI.");
		}
	}
	
	if (WaitVideoSyncSGI == nullptr) {
		WaitVideoSyncSGI = reinterpret_cast<WaitVideoSyncSGI_sig>(glXGetProcAddress(reinterpret_cast<GLubyte const*>("glXWaitVideoSyncSGI")));
		if (!WaitVideoSyncSGI) {
			// throw GLX::InitializationError("Failed to load glXWaitVideoSyncSGI.");
		}
	}
}




bool framebuffer_supports_rgba(::Display* display, ::GLXFBConfig framebuffer)
{
	assert(display != nullptr);
	assert(framebuffer != nullptr);


	int result = False;
	glXGetFBConfigAttrib(display, framebuffer, GLX_BIND_TO_TEXTURE_RGBA_EXT, &result);

	return result == True;
}



} // namespace GLX

