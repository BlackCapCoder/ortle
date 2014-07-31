// #include "damage.hpp"

// #include "exceptions.hpp"

// #include "../utility/trace.hpp"

// #include <X11/Xlib.h>
// #include <X11/extensions/Xdamage.h>
// #include <X11/extensions/Xfixes.h>

// #include <cassert>

// #include <utility>




// namespace X11 {


// Damage::Damage()
// 	: m_display(nullptr)
// 	, m_drawable(None)
// 	, m_damage(None)
// {}


// Damage::Damage(::Display* display, ::Drawable drawable, int level)
// 	: m_display(display)
// 	, m_drawable(drawable)
// 	, m_damage(None)
// {
// 	assert(display != nullptr);
// 	assert(drawable != None);


// 	// TRACE("creating damage for drawable", drawable);

// 	::Damage damage = XDamageCreate(display, drawable, level);

// 	if (!damage) {
// 		throw InitializationError("Failed to create new Damage.");
// 	}

// 	m_damage = damage;
// }




// Damage::Damage(Damage&& other)
// 	: Damage()
// {
// 	swap(*this, other);
// }


// Damage& Damage::operator=(Damage&& other)
// {
// 	swap(*this, other);
// 	return *this;
// }




// Damage::~Damage()
// {
// 	if (m_display != nullptr) {

// 		// TRACE("destroying damage", m_damage);

// 		XDamageDestroy(m_display, m_damage);
// 	}
// }




// void swap(Damage& first, Damage& second)
// {
// 	using std::swap;

// 	swap(first.m_display, second.m_display);
// 	swap(first.m_drawable, second.m_drawable);
// 	swap(first.m_damage, second.m_damage);
// }


// } // namespace X11

