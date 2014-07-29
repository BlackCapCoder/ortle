# Ortle

Ortle is a simple compositing manager for X.  Its primary purpose is to
eliminate tearing effects while impacting performance as little as possible.
It also, though somewhat incidentally, makes windows with ARGB visuals appear
correctly.  It currently does not provide any eye candy, and possibly never
will.


## Current Status: UNSTABLE/TESTING

Ortle has been stable for me with an Nvidia graphics card and the proprietary 
driver for months.  It is not stable when I use nouveau (my GPU locks up), and 
it apparently doesn't work at all on Intel Haswell hardware.  **Please be 
careful when giving it a try!**


## Requirements

* a video card and driver that support
[OpenGL 3.3](https://en.wikipedia.org/wiki/OpenGL#OpenGL_3.3)
* a compiler that supports C++11
* an X server that supports the following extensions:
  * XComposite 0.4
  * XFixes 2.0
  * XShape 1.1
  * glX 1.4
* possibly other things.

It is important to note that Ortle relies upon your graphics driver to work. If
`glXSwapIntervalEXT` fails to enforce vsync, or if the implementation of
`glXBindTexImageEXT` requires extra synchronization on my part (which, looking
at the spec, it might), you might be better served by an alternative like
compton.


## Building

* `$ git clone https://github.com/aoba1217/ortle.git`
* `$ cd ortle`
* `$ make`


## Alternatives
* [xcompmgr](http://cgit.freedesktop.org/xorg/app/xcompmgr/) by Keith Packard
* [compton](https://github.com/chjj/compton) by Christopher Jeffrey,
Richard Grenville, et (possibly) al.  
