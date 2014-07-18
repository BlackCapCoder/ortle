#ifndef ORTLE_X11_ERROR_HANDLER_HPP
#define ORTLE_X11_ERROR_HANDLER_HPP


#include <X11/Xlib.h>



namespace X11 {


class ErrorHandler {

public:

	using Function = int (*)(::Display*, ::XErrorEvent*);


public:

	explicit ErrorHandler(Function handler);

	ErrorHandler(ErrorHandler&& other);
	ErrorHandler& operator=(ErrorHandler&& other);

	~ErrorHandler();

	friend void swap(ErrorHandler& first, ErrorHandler& second);


private:

	Function m_original_handler;
	Function m_handler;

};


} // namespace X11


#endif

