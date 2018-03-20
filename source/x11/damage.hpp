#ifndef ORTLE_X11_DAMAGE_HPP
#define ORTLE_X11_DAMAGE_HPP


#include <X11/Xlib.h>
#include <X11/extensions/Xdamage.h>
#include <X11/extensions/Xfixes.h>




namespace X11 {


class Damage {

public:

	Damage();
	Damage(::Display* display, ::Drawable drawable, int level);

	Damage(Damage&& other);
	Damage& operator=(Damage&& other);

	~Damage();

	friend void swap(Damage& first, Damage& second);


public:

	operator ::Damage() const
	{
		return m_damage;
	}


private:

	::Display* m_display;
	::Drawable m_drawable;
	::Damage m_damage;

};


} // namespace X11


#endif

