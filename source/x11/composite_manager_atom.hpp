#ifndef ORTLE_X11_COMPOSITE_MANAGER_ATOM_HPP
#define ORTLE_X11_COMPOSITE_MANAGER_ATOM_HPP


#include <X11/Xatom.h>
#include <X11/Xlib.h>




namespace X11 {


class CompositeManagerAtom {

public:

	CompositeManagerAtom(::Display* display, int screen, ::Window owner);

	CompositeManagerAtom(CompositeManagerAtom&& other);
	CompositeManagerAtom& operator=(CompositeManagerAtom&& other);

	~CompositeManagerAtom();

	friend void swap(CompositeManagerAtom& first, CompositeManagerAtom& second);


private:

	::Display* m_display;
	int m_screen;
	::Window m_owner;
	::Atom m_composite_manager_atom;

};


} // namespace X11


#endif

