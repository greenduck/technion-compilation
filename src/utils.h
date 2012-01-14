#ifndef _UTILS_H
#define _UTILS_H

#include <string>

class CException
{
public:
	CException(const string message)
		:m_string(message)
	{
	}

	~CException()
	{
	}

	inline const string String()
	{
		return m_string;
	}

private:
	const string m_string;
};

#endif	// _UTILS_H

