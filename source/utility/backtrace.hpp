#ifndef UTILITY_BACKTRACE_HPP
#define UTILITY_BACKTRACE_HPP


#include <iostream>




namespace Utility {


class Backtrace {

public:

	Backtrace();
	explicit Backtrace(int file_descriptor);

	Backtrace(Backtrace&& other);
	Backtrace& operator=(Backtrace&& other);

	~Backtrace();

	friend void swap(Backtrace& first, Backtrace& second);


public:

	int count() const
	{
		return m_count;
	}

	char** symbols() const
	{
		return m_symbols;
	}


private:

	char** m_symbols;
	int m_count;

};




std::ostream& operator<<(std::ostream& output, Backtrace const& backtrace);




} // namespace Utility


#endif
