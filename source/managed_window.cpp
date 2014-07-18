#include "managed_window.hpp"

#include <X11/Xlib.h>

#include <utility>




ManagedWindow::ManagedWindow(Window window)
	: m_window(window)
{}




ManagedWindow::ManagedWindow(ManagedWindow&& other)
	: m_window(None)
{
	swap(*this, other);
}


ManagedWindow& ManagedWindow::operator=(ManagedWindow&& other)
{
	swap(*this, other);
	return *this;
}




void swap(ManagedWindow& first, ManagedWindow& second)
{
	using std::swap;

	swap(first.m_window, second.m_window);
}

