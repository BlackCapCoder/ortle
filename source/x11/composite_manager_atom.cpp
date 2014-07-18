#include "composite_manager_atom.hpp"

#include "exceptions.hpp"

#include <X11/Xatom.h>
#include <X11/Xlib.h>

#include <cassert>

#include <string>
#include <utility>




namespace {


Atom get_composite_manager_atom(::Display* display, int screen)
{
	std::string atom_name("_NET_WM_CM_S");
	atom_name += std::to_string(screen);
	return XInternAtom(display, atom_name.c_str(), False);
}


} // namespace




namespace X11 {


CompositeManagerAtom::CompositeManagerAtom(::Display* display, int screen, ::Window owner)
	: m_display(display)
	, m_screen(screen)
	, m_owner(owner)
	, m_composite_manager_atom(None)
{
	assert(display != nullptr);
	assert(screen >= 0);
	assert(owner != None);


	::Atom composite_manager_atom = get_composite_manager_atom(display, screen);

	if (!composite_manager_atom) {
		throw InitializationError("Could not create composite manager atom.");
	}

	if (XGetSelectionOwner(display, composite_manager_atom) != None) {
		throw InitializationError("Composite manager atom already has an owner.");
	}

	XSetSelectionOwner(display, composite_manager_atom, owner, CurrentTime);

	m_composite_manager_atom = composite_manager_atom;
}




CompositeManagerAtom::CompositeManagerAtom(CompositeManagerAtom&& other)
	: m_display(nullptr)
	, m_screen(0)
	, m_owner(None)
	, m_composite_manager_atom(None)
{
	swap(*this, other);
}


CompositeManagerAtom& CompositeManagerAtom::operator=(CompositeManagerAtom&& other)
{
	swap(*this, other);
	return *this;
}




CompositeManagerAtom::~CompositeManagerAtom()
{
	if (m_display != nullptr) {
		XSetSelectionOwner(m_display, m_composite_manager_atom, None, CurrentTime);
	}
}




void swap(CompositeManagerAtom& first, CompositeManagerAtom& second)
{
	using std::swap;

	swap(first.m_display, second.m_display);
	swap(first.m_screen, second.m_screen);
	swap(first.m_owner, second.m_owner);
	swap(first.m_composite_manager_atom, second.m_composite_manager_atom);
}


} // namespace X11

