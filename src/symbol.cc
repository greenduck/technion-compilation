#include <strings.h>
#include <iostream>

#include "symbol.h"
#include "storage.h"
#include "semantic_rules.h"
#include "utils.h"



SymbolScope::SymbolScope(SymbolScope *parent)
	:CScopeInfo<CSymbol>(parent)
{
	// mirror register allocations
	regUsage[CSymbol::INTEGER] = new CRegAlloc(NREGS);
	regUsage[CSymbol::REAL] = new CRegAlloc(NREGS);
}

SymbolScope::~SymbolScope()
{
	delete regUsage[CSymbol::INTEGER];
	delete regUsage[CSymbol::REAL];
}



CSymbol::CSymbol(const char *label, TypeID typeID)
	:LinkedListNode(),
	 m_label(label),
	 m_typeID(typeID),
	 m_valueStatus(INVALID),
	 m_value(""),
	 m_memalloc(-1),
	 m_regalloc(-1)
{
}

CSymbol::CSymbol(const char *label, const char *value, TypeID typeID)
	:LinkedListNode(),
	 m_label(label),
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
	return Last()->m_label;
}

CSymbol::TypeID CSymbol::Type()
{
	return Last()->m_typeID;
}

void CSymbol::SetValue(const string& value)
{
	BUG_IF((Last()->m_valueStatus == CONST), "Attempt to change constant value");

	int flag = ((m_typeID == INTEGER || m_typeID == LABEL) << 1) | (DiscoverType(value.c_str()) == INTEGER);
	switch (flag)
	{
	case 3:	Last()->m_value = value;
			break;
	case 2:	Last()->m_value = value.substr(0, value.find_first_of("."));
			break;
	case 1:	Last()->m_value = value + ".0";
			break;
	case 0:	Last()->m_value = value;
			break;
	}

	Last()->m_valueStatus = VALID;
}

void CSymbol::SetValue(const char *value)
{
	SetValue(string(value));
}

string CSymbol::GetValue()
{
	BUG_IF((Last()->m_valueStatus == INVALID), "Request undetermined value: " + Last()->m_label);

	return Last()->m_value;
}

bool CSymbol::ValueIsSet()
{
	return (Last()->m_valueStatus >= VALID);
}

void CSymbol::DiscardValue()
{
	BUG_IF((Last()->m_valueStatus == CONST), "Attempt to discard constant value");

	Last()->m_valueStatus = INVALID;
}

void CSymbol::Patch(CSymbol *target)
{
	BUG_IF((Next() != NULL), "Attempt to override write once value");
	BUG_IF((m_valueStatus == CONST), "Attempt to override constant value");	// (?) possibly in some propagation optimizations, it is valid

	AddNext(target);
}

void CSymbol::Register(int reg)
{
	BUG_IF((m_regalloc != CRegAlloc::NOREG), "Attempt to override write once value");
	m_regalloc = reg;
}

int CSymbol::Register()
{
	return m_regalloc;
}

ostream& operator<<(ostream& os, const CSymbol& sym)
{
	const char *reg_to_string[] = {"I", "R"};

	CSymbol *s = ((CSymbol *)&sym)->Last();	/* cast away 'const' ... */
	if (s->m_valueStatus == CSymbol::CONST || s->m_valueStatus == CSymbol::VALID) {
		os << s->m_value;
	}
	else if (s->m_regalloc != CRegAlloc::NOREG) {
		os << reg_to_string[s->Type()] << s->m_regalloc;
	}
	else {
		os << s->m_label;
	}

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
	CSymbol *sym = new CSymbol(name, typeID);
	sym->Register( currentScope->regUsage[typeID]->Mirror( regPool[typeID]->AcquireOrThrow() ) );
	return sym;
}

CSymbol *newTemp(CSymbol::TypeID typeID)
{
	static int tempIndex = 0;
	char name[64];

	snprintf(name, sizeof(name), "@tmp.%d", tempIndex++);
	CSymbol *sym = new CSymbol(name, typeID);
	sym->Register( currentScope->regUsage[typeID]->Mirror( regPool[typeID]->AcquireOrThrow() ) );
	return sym;
}


/* TODO:
 * build some persistent map that would allow re-using constant values, 
 * instead of allocating repeating values 
 */
CSymbol *newConst(const char *value)
{
	return new CSymbol("", value, CSymbol::DiscoverType(value));
}

CSymbol *newConst(const string& value)
{
	return newConst(value.c_str());
}

CSymbol *newConst(int value)
{
	return new CSymbol("", CSymbol::NumericToString(value), CSymbol::INTEGER);
}

CSymbol *newConst(float value)
{
	return new CSymbol("", CSymbol::NumericToString(value), CSymbol::REAL);
}


CSymbol *regSymbol(const char *label, CSymbol::TypeID typeID, int regID)
{
	CSymbol *sym = new CSymbol(label, typeID);
	sym->Register(regID);
	return sym;
}


CSymbol *newLabel(const char *prefix)
{
	static int labelIndex = 0;
	char name[64];

	snprintf(name, sizeof(name), "L%s.%d", prefix, labelIndex++);
	return new CSymbol(name, CSymbol::LABEL);
}



CFuncSymbol::CFuncSymbol(const char *label, TypeID retType)
	:CSymbol(label, "2", retType)
{
}

CFuncSymbol::~CFuncSymbol()
{
}

void CFuncSymbol::AddArgument(CSymbol *arg)
{
	m_arguments.push_back(arg);
}

