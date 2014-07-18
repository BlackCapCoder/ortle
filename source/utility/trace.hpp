#ifndef UTILITY_TRACE_HPP
#define UTILITY_TRACE_HPP




#ifdef NDEBUG

	#define TRACE(...) ((void)0)

#else

	#include <iostream>

	namespace Utility {

	class Tracer {

	public:

		Tracer(std::ostream& ostream, char const* file, int line, char const* function)
			: m_ostream(ostream)
		{
			m_ostream << file << " :: " << line << " :: " << function;
		}

		template<typename T, typename... U>
		Tracer& operator()(T const& first, U const&... rest)
		{
			m_ostream << " :: " << first;
			return operator()(rest...);
		}

		Tracer& operator()()
		{
			m_ostream << '\n';
			return *this;
		}

	private:

		std::ostream& m_ostream;

	};

	} // namespace Utility

	#define TRACE(...) Utility::Tracer(std::clog, __FILE__, __LINE__, __func__)(__VA_ARGS__)

#endif




#endif
