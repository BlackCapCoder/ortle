#include "exceptions.hpp"
#include "ortle.hpp"

#include "glx/exceptions.hpp"

#include "opengl/core330.hpp"
#include "opengl/exceptions.hpp"

#include "x11/exceptions.hpp"

#include <iostream>
#include <stdexcept>




template<typename Output>
void print(Output& output)
{
	output << "\n";
}


template<typename Output, typename First, typename... Rest>
void print(Output& output, First const& first, Rest const&... rest)
{
	output << first;
	print(output, rest...);
}


template<typename... Args>
void error(Args const&... args)
{
	print(std::cerr, "Ortle: ERROR: ", args...);
}




int main(int argc, char** argv)
{
	try {
		Ortle ortle(argc, argv);
		ortle.run();

		return 0;
	}

	catch (GLX::InitializationError& e) {
		error("GLX: ", e.what());
	}

	catch (OpenGL::StateError& e) {

		error("OpenGL: ", e.what());

		switch (e.code()) {

			case gl::INVALID_ENUM:
				error("OpenGL: gl::INVALID_ENUM");
				break;

			case gl::INVALID_VALUE:
				error("OpenGL: gl::INVALID_VALUE");
				break;

			case gl::INVALID_OPERATION:
				error("OpenGL: gl::INVALID_OPERATION");
				break;

			case gl::INVALID_FRAMEBUFFER_OPERATION:
				error("OpenGL: gl::INVALID_FRAMEBUFFER_OPERATION");
				break;

			case gl::OUT_OF_MEMORY:
				error("OpenGL: gl::OUT_OF_MEMORY");
				break;

			default:
				error("OpenGL: code ", e.code());
		}
	}

	catch (X11::InitializationError& e) {
		error("X11: ", e.what());
	}

	catch (X11::IncompatibleVersion& e) {
		error("Incompatble version of X extension ", e.what());
	}

	catch (X11::MissingExtension& e) {
		error("Required X extension ", e.what(), " not found.");
	}

	catch (InitializationError& e) {
		error(e.what());
	}

	catch (std::exception& e) {
		error("Foreign exception caught:");
		error(e.what());
	}

	catch (...) {
		error("Unknown exception caught.");
	}

	error("Shutting down...");
	return -1;
}

