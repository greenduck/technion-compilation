#include <iostream>

#include "symbol.h"
#include "utils.h"

CSymbol::CSymbol(const char *label, TypeID typeID)
	:m_label(label),
	 m_typeID(typeID),
	 m_valueStatus(INVALID),
	 m_memalloc(-1),
	 m_regalloc(-1)
{
}

// CSymbol::CSymbol(const char *value, TypeID typeID)
// 	:m_label(""),
// 	 m_typeID(typeID),
// 	 m_valueStatus(CONST),
// 	 m_memalloc(-1),
// 	 m_regalloc(-1)
// {
// }

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
	if (m_valueStatus == CONST)
		throw CBugException("Attempt to change constant value");

	m_value = string(value);
	m_valueStatus = VALID;
}

string CSymbol::GetValue()
{
	if (m_valueStatus == INVALID)
		throw CBugException("Request undetermined value: " + m_label);

	return m_value;
}

bool CSymbol::ValueIsSet()
{
	return (m_valueStatus >= VALID);
}

void CSymbol::DiscardValue()
{
	if (m_valueStatus == CONST)
		throw CBugException("Attempt to discard constant value");

	m_valueStatus = INVALID;
}

ostream& operator<<(ostream& os, const CSymbol& sym)
{
	// TODO: with const. folding on, display value whenever it is known (VALID, CONST)
	os << ((sym.m_valueStatus != CSymbol::CONST) ? sym.m_label : sym.m_value);
	return os;
}

