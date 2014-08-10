#ifndef GLX_FUNCTIONS_HPP
#define GLX_FUNCTIONS_HPP


#include "../opengl/core330.hpp"

#include <X11/Xlib.h>

#include <GL/glx.h>




namespace GLX {


extern ::GLXContext (*CreateContextAttribsARB)(::Display*, ::GLXFBConfig, ::GLXContext, Bool, int const*);

extern void (*BindTexImageEXT)(::Display*, ::GLXDrawable, int, int const*);
extern void (*ReleaseTexImageEXT)(::Display*, ::GLXDrawable, int);
extern void (*SwapIntervalEXT)(::Display*, ::GLXDrawable, int);
extern int (*SwapIntervalMESA)(int);

extern int (*GetVideoSyncSGI)(unsigned int*);
extern int (*WaitVideoSyncSGI)(int, int, unsigned int*);


void load_functions();


bool framebuffer_supports_rgba(::Display* display, ::GLXFBConfig framebuffer);


} // namespace GLX


#endif

