#include "input_only_window.hpp"

#include "managed_window.hpp"

#include "utility/trace.hpp"

#include <X11/Xlib.h>

#include <cassert>

#include <utility>




InputOnlyWindow::InputOnlyWindow(XCreateWindowEvent const& event)
	: ManagedWindow(event.window)
{
	assert(event.window != None);

	TRACE("managing input-only window", event.window);
}




InputOnlyWindow::InputOnlyWindow(InputOnlyWindow&& other)
	: ManagedWindow(None)
{
	swap(*this, other);
}


InputOnlyWindow& InputOnlyWindow::operator=(InputOnlyWindow&& other)
{
	swap(*this, other);
	return *this;
}




InputOnlyWindow::~InputOnlyWindow()
{
	if (*this != None) {
		TRACE("stopping management of input-only window", *this);
	}
}



void swap(InputOnlyWindow& first, InputOnlyWindow& second)
{
	using std::swap;

	swap(static_cast<ManagedWindow&>(first), static_cast<ManagedWindow&>(second));
}

