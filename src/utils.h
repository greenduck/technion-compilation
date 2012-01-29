#ifndef _UTILS_H
#define _UTILS_H

#include <string>
#include <iostream>

using namespace std;

// basic exception class
class CException
{
public:
	CException(const string message)
		:m_string(message)
	{
	}

	virtual ~CException()
	{
	}

	virtual const string String()
	{
		return m_string;
	}

private:
	const string m_string;
};

// error in code being compiled
class CCompilationException : public CException
{
public:
	CCompilationException(const string message)
		: CException(message)
	{
	}
};

// error in compiler code
class CBugException : public CException
{
public:
	CBugException(const string message)
		: CException(message)
	{
	}
};



// output objects
extern ostream &machout;
extern ostream &dbgout;

#endif	// _UTILS_H

