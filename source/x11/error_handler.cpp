#include "error_handler.hpp"

#include <X11/Xlib.h>

#include <cassert>

#include <utility>




namespace X11 {


ErrorHandler::ErrorHandler(Function handler)
	: m_original_handler(XSetErrorHandler(handler))
	, m_handler(handler)
{
	assert(handler != nullptr);
}




ErrorHandler::ErrorHandler(ErrorHandler&& other)
	: m_original_handler(nullptr)
	, m_handler(nullptr)
{
	swap(*this, other);
}


ErrorHandler& ErrorHandler::operator=(ErrorHandler&& other)
{
	swap(*this, other);
	return *this;
}




ErrorHandler::~ErrorHandler()
{
	if (m_handler != nullptr) {
		XSetErrorHandler(m_original_handler);
	}
}




void swap(ErrorHandler& first, ErrorHandler& second)
{
	using std::swap;

	swap(first.m_original_handler, second.m_original_handler);
	swap(first.m_handler, second.m_handler);
}


} // namespace X11

