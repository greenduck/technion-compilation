#include <strings.h>
#include <iostream>

#include "symbol.h"
#include "utils.h"

CSymbol::CSymbol(const char *label, TypeID typeID)
	:m_label(label),
	 m_typeID(typeID),
	 m_valueStatus(INVALID),
	 m_value(""),
	 m_memalloc(-1),
	 m_regalloc(-1)
{
}

CSymbol::CSymbol(const char *label, const char *value, TypeID typeID)
	:m_label(label),
	 m_typeID(typeID),
	 m_valueStatus(CONST),
	 m_value(value),
	 m_memalloc(-1),
	 m_regalloc(-1)
{
}

CSymbol::~CSymbol()
{
}

string CSymbol::Label()
{
	return m_label;
}

CSymbol::TypeID CSymbol::Type()
{
	return m_typeID;
}

void CSymbol::SetValue(const char *value)
{
	BUG_IF((m_valueStatus == CONST), "Attempt to change constant value");

	m_value = string(value);
	m_valueStatus = VALID;
}

string CSymbol::GetValue()
{
	BUG_IF((m_valueStatus == INVALID), "Request undetermined value: " + m_label);

	return m_value;
}

bool CSymbol::ValueIsSet()
{
	return (m_valueStatus >= VALID);
}

void CSymbol::DiscardValue()
{
	BUG_IF((m_valueStatus == CONST), "Attempt to discard constant value");

	m_valueStatus = INVALID;
}

ostream& operator<<(ostream& os, const CSymbol& sym)
{
	// TODO: with const. folding on, display value whenever it is known (VALID, CONST)
	os << ((sym.m_valueStatus != CSymbol::CONST) ? sym.m_label : sym.m_value);
	return os;
}

CSymbol::TypeID CSymbol::DiscoverType(const char *value)
{
	return (index(value, '.') == NULL) ? INTEGER : REAL;
}

const char *CSymbol::NumericToString(int value)
{
	static char toString[8][64];
	static int index = -1;
	index = (index + 1) % 8;
	snprintf(toString[index], sizeof(toString[0]), "%d", value);
	return toString[index];
}

const char *CSymbol::NumericToString(float value)
{
	static char toString[8][64];
	static int index = -1;
	index = (index + 1) % 8;
	// FIXME: the '%g' thing will not work well for larger numbers
	snprintf(toString[index], sizeof(toString[0]), "%g%s", value, (value == (int)value ? ".0" : ""));
	return toString[index];
}


CSymbol *newSymbol(const char *name, CSymbol::TypeID typeID)
{
	return new CSymbol(name, typeID);
}

CSymbol *newTemp(CSymbol::TypeID typeID)
{
	static int tempIndex = 0;
	char name[64];

	snprintf(name, sizeof(name), "@tmp.%d", tempIndex++);
	return new CSymbol(name, typeID);
}


CSymbol *newConst(const char *value)
{
	return new CSymbol("", value, CSymbol::DiscoverType(value));
}

CSymbol *newConst(int value)
{
	return new CSymbol("", CSymbol::NumericToString(value), CSymbol::INTEGER);
}

CSymbol *newConst(float value)
{
	return new CSymbol("", CSymbol::NumericToString(value), CSymbol::REAL);
}


CSymbol *newLabel(const char *prefix)
{
	static int labelIndex = 0;
	char name[64];

	snprintf(name, sizeof(name), "L%s.%d", prefix, labelIndex++);
	return new CSymbol(name, CSymbol::LABEL);
}

