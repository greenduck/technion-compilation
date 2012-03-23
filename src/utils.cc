#include <execinfo.h>
#include <stdlib.h>
#include <cxxabi.h>
#include "utils.h"



/* Some extreme coding exercising stack tracing and C++ name demangling
 * Reference:
 * http://tombarta.wordpress.com/2008/08/01/c-stack-traces-with-gcc/
 */
CBugException::CBugException(const string message)
	:CException(message)
{
	const int maxdepth = 16;
	void *buff[maxdepth];
	char **names;
	int depth;

	depth = backtrace(buff, maxdepth);
	names = backtrace_symbols(buff, depth);

	for (int i = 0; i < depth && names != NULL; ++i) {
		m_string += "\n";

		char *mangled_name = NULL;
		char *begin = NULL;
		char *end = NULL;

		for (char *p = names[i]; *p && end == NULL; ++p) {
			switch (*p)
			{
			case '(':	mangled_name = p;	break;
			case '+':	begin = p;			break;
			case ')':	end = p;			break;
			}
		}

		if (mangled_name && begin && end && mangled_name < begin) {
			int status;

			*mangled_name++ = '\0';
			*begin++ = '\0';
			*end++ = '\0';

			char *demangled_name = abi::__cxa_demangle(mangled_name, 0, 0, &status);
			if (status == 0)
				m_string += demangled_name;
			else
				m_string += mangled_name;

			free(demangled_name);
		}
		else {
			m_string += names[i];
		}
	}
	free(names);
}

ostream& operator<<(ostream& os, const CException& ex)
{
	os << ex.m_string;
	return os;
}



// output objects
filebuf outfileFb;
ostream machout(&outfileFb);

filebuf dbgfileFb;
ostream dbgout(&dbgfileFb);

bool constPropagation;
char *outputPath;

