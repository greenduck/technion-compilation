#ifndef _UTILS_H
#define _UTILS_H

#include <stdio.h>
#include <stdarg.h>

#include <string>
#include <iostream>



// flex / bison exported symbols
int yylex(void);
int yyparse(void);
void yyerror(const char *);

extern int yylineno;
extern char *yytext;
extern FILE *yyin;



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

	friend ostream& operator<<(ostream& os, const CException& ex);

private:
	const string m_string;
};

// error in code being compiled
class CCompilationException : public CException
{
public:
	CCompilationException(const string message)
		:CException(message)
	{
	}
};

// error in compiler code
class CBugException : public CException
{
public:
	CBugException(const string message)
		:CException(message)
	{
	}
};

#define BUG_IF(cond, msg)													\
	do {																	\
		if (cond) {															\
			char temp[64];													\
			snprintf(temp, sizeof(temp), "[%s:%d]: ", __FILE__, __LINE__);	\
			throw CBugException(string(temp) + string(msg));				\
		}																	\
	} while(0)



// output objects
extern ostream &machout;
extern ostream &dbgout;

#endif	// _UTILS_H

