# Design Notes

This document is a rough overview of Ortle's source.  It may be of use to
to anyone looking at it for the first time, but its main purpose is to serve as
a refresher for me after I inevitably take time away from the project and
forget everything I have done.


## Classes

Ortle is composed of three categories of classes: *Resource Managers*, which
are numerous but simple; *Compound Classes*, which are few but complex; and
*Things Not in the Other Two Categories*, which do not fit well in to the other
two categories.


### Resource Managers

The most numerous and tedious of the classes, Resource Managers acquire a
resource upon construction (e.g. an X Pixmap or an OpenGL texture) and release
it upon destruction.  These are movable in the C++11 sense, but not copyable
because I am lazy and it never came up.  You can probably look at any one of
these and immediately understand the rest.

Generally, these classes don't provide an API beyond a type conversion operator
that allows them to be implicitly used in C API calls (e.g. given `X11::Display
display(NULL);`, one could then call `int screen = XDefaultScreen(display);`).
Note that this does not work for macros (e.g.
`int screen = DefaultScreen(display);` would fail).

Some of these classes can be empty - created without acquiring a resource - to
allow flexibility when constructing compound objects.  For `OpenGL` classes,
this is done by passing `0` to the constructor, for others this is done with a
constructor that accepts no arguments.


##### In the GLX namespace:

* `Context` - GLXContext
* `Pixmap` - GLXPixmap
* `Window` - GLXWindow


##### In the OpenGL namespace:

* `Buffer` - a generic OpenGL buffer (`gl::GenBuffers`)
* `Program` - an OpenGL program (`gl::CreateProgram`)
* `Shader` - an OpenGL shader (`gl::CreateShader`)
* `Texture` - an OpenGL texture (`gl::GenTextures`)
* `VertexArray` - an OpenGL vertex array (`gl::GenVertexArrays`)


##### In the X11 namespace:

* `Colormap`
* `ComposteManagerAtom` - Ownership of the _NET_WM_CM_S? atom
* `CompositeOverlay` - `XComposite{Get|Release}OverlayWindow`
* `Display` - Xlib Display pointer
* `ErrorHandler` - `XSetErrorHandler` (restores the old one on destruction)
* `Pixmap`
* `RectangleList` - List of bounding rectangles of a shaped window
* `VisualInfo` - XVisualInfo
* `Window`


### Compound Classes

These are the classes that define the behavior of the program.

* `FramebufferCache` - stores a list of `GLXFBConfig`s and a mapping that
associates a Visual ID with an entry in that list.  This provides a quick way
to find a compatible `GLXFBConfig` each time a window is added.

* `InputOnlyWindow` - class derived from the `ManagedWindow` base.  Occupies a
spot in `WindowManager`'s stack without storing all the information associated
with a drawable window.

* `InputOutputWindow` - class derived from the `ManagedWindow` base.  Provides
everything the `Renderer` needs to draw a given window on the screen.  Responds
to (dispatched) Xlib events to keep its information current.

* `ManagedWindow` - not technically a compound class, but rather a base class
for the few window types managed by `WindowManager`.

* `Ortle` - the main class of the program.  Sets up everything and provides the
main loop in `Ortle::run`.

* `OutputWindow` - the window and glX context where everything is drawn to.
This is parented to the X Composite overlay window, and maintains the same
dimensions as the root window.

* `Renderer` - basically an OpenGL program and the OpenGL calls required to use
that program to draw a `ManagedWindow` on `OutputWindow`'s context.

* `Root` - class derived from the `ManagedWindow` base.  Manages a copy of the
root window background (given by `X11::WallpaperPixmap`) and the
`OpenGL::Texture` bound to it.

* `WindowManager` - maintains a list of managed windows (to which it dispatches
certain events).  This list is used to determine in what order the windows are
rendered.


### Things Not in the Other Two Categories

* `*/exceptions.hpp` - scroll down.

* `glx/functions.?pp` - the addresses of certain glX* calls have to be looked
up at runtime before they can be used.  This provides that functionality, as
well as a number (specifically, that number is one) of helper GLX functions.

* `opengl/core330.?pp` - this is a pair of files that was generated with
[glLoadGen](https://bitbucket.org/alfonse/glloadgen/wiki/Home) at some point in
this decade.  It provides access to the OpenGL 3.3 API.

* `utility/backtrace.?pp` - debug helper that generates a stack trace.  This is
mostly useless.

* `utility/trace.hpp` - allows me to pollute my code with `TRACE()` calls that
tell me what Ortle is doing.  Invaluable in determining all the ways that X
decides to be insane.

* `x11/extension.?pp` - checks for the presense of an extension (e.g.
XComposite), checks its version, and stores its error and event base codes.

* `x11/functions.?pp` - helper function.

* `x11/geometry.?pp`, `x11/shape_extents.?pp` and `x11/wallpaper_pixmap.?pp` -
querying X for a certain value is either difficult (WallpaperPixmap) or comes
with a lot of baggage (Geometry, ShapeExtents).  These are function calls
pretending to be classes to make other code look prettier.


## Exceptions

I understand the theory of when exceptions should be used, and I even
understand how to code with them in mind (okay, somewhat), but almost every
single one in Ortle is fatal.  I might be fooling myself.

* `GLX::InitializationError` - thrown when any of the GLX classes fail to
acquire their resource (a `GLXContext`, `GLXPixmap` or `GLXWindow`) or when
`GLX::load_functions` fails to load a needed function (e.g.
`glXBindTexImageEXT`).

* `OpenGL::ShaderError` and `OpenGL::ProgramError` - these mean there are bugs
in my shader code that prevented them from compiling or linking.

* `OpenGL::StateError` - each iteration of the main loop checks the OpenGL
state for errors.  If it finds one, it throws one of these.  This can mean
either a misuse of the OpenGL API on my part, or a runtime error (e.g. running
out of memory) that left the OpenGL state unusable.

* `X11::InitializationError` - thrown when an X11 class fails to acquire its
resource (a `Colormap`, `Display`, `Pixmap`...).

* `::InitializationError` - one of the compound classes failed to construct.

* `::FramebufferError` - something went wrong when trying to find a compatible
GLXFBConfig.
