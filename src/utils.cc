#include "utils.h"



ostream& operator<<(ostream& os, const CException& ex)
{
	os << ex.m_string;
	return os;
}



// output objects
ostream &machout(std::cout);
ostream &dbgout(std::cout);

