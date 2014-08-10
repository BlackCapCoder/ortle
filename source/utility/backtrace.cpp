#include "backtrace.hpp"

#include <execinfo.h>
#include <unistd.h>

#include <cassert>
#include <cstdlib>

#include <iostream>
#include <utility>




#ifndef UTILITY_BACKTRACE_DEPTH
#define UTILITY_BACKTRACE_DEPTH 64
#endif




#ifdef UTILITY_BACKTRACE_DEMANGLE_CPP


#include <cxxabi.h>

#include <cstddef>
#include <cstdlib>

#include <sstream>
#include <string>




namespace {


std::string demangled_line(char const* input_line)
{
	std::size_t function_begin = 0;
	std::size_t function_end = 0;

	std::string mangled_function;

	int status;
	char* demangled_function = nullptr;

	std::stringstream result;



	for (std::size_t i = 0; input_line[i] != '\0'; ++i) {
		if (input_line[i] == '(') {
			function_begin = i + 1;
			// don't break: we want the last one in the string
		}
	}

	if (function_begin == 0 || input_line[function_begin] == '\0') {
		return input_line;
	}


	for (std::size_t i = function_begin; input_line[i] != '\0'; ++i) {
		if (input_line[i] == '+') {
			function_end = i;
			break;
		}
	}

	if (function_end == 0) {
		return input_line;
	}


	mangled_function = std::string(input_line + function_begin, input_line + function_end);
	demangled_function = abi::__cxa_demangle(mangled_function.c_str(), NULL, NULL, &status);

	if (status != 0 || demangled_function == nullptr) {
		std::free(demangled_function);
		return input_line;
	}


	result.write(input_line, function_begin);
	result << demangled_function;
	result << (input_line + function_end);

	std::free(demangled_function);

	return result.str();
}


} // namespace


#else


namespace {


char const* demangled_line(char const* input_line)
{
	return input_line;
}


} // namespace


#endif




namespace Utility {


Backtrace::Backtrace()
	: m_symbols(nullptr)
	, m_count(0)
{
	void* addresses[UTILITY_BACKTRACE_DEPTH] = { nullptr };
	int num_addresses = 0;

	num_addresses = backtrace(addresses, UTILITY_BACKTRACE_DEPTH);
	char** symbols = backtrace_symbols(addresses, num_addresses);

	if (symbols != nullptr) {
		m_symbols = symbols;
		m_count = num_addresses;
	}
}


Backtrace::Backtrace(int file_descriptor)
	: m_symbols(nullptr)
	, m_count(0)
{
	void* addresses[UTILITY_BACKTRACE_DEPTH] = { nullptr };
	int num_addresses = 0;

	num_addresses = backtrace(addresses, UTILITY_BACKTRACE_DEPTH);
	backtrace_symbols_fd(addresses, num_addresses, file_descriptor);
}




Backtrace::Backtrace(Backtrace&& other)
	: m_symbols(nullptr)
	, m_count(0)
{
	swap(*this, other);
}


Backtrace& Backtrace::operator=(Backtrace&& other)
{
	swap(*this, other);
	return *this;
}




Backtrace::~Backtrace()
{
	if (m_symbols != nullptr) {
		std::free(m_symbols);
	}
}




void swap(Backtrace& first, Backtrace& second)
{
	using std::swap;

	swap(first.m_symbols, second.m_symbols);
}




std::ostream& operator<<(std::ostream& output, Backtrace const& backtrace)
{
	// note:
	// we start at index 1 because index 0 is always Backtrace's constructor.

	int count = backtrace.count();

	if (count > 1) {

		char** symbols = backtrace.symbols();

		output << "Execution path:\n";

		for (int i = 1; i < count; ++i) {
			output << demangled_line(symbols[i]) << '\n';
		}
	}

	return output;
}


} // namespace Utility
