#ifndef _SYMBOL_H
#define _SYMBOL_H

#include <map>
#include <list>
#include <string>

#include "utils.h"
#include "linked_list.h"

using namespace std;

// data structure for handling nested scopes
template <typename TSym>
class CScopeInfo
{
public:
	CScopeInfo(CScopeInfo<TSym> *parent)
		:m_superscope(parent)
	{
		if (m_superscope == NULL) {
			m_nesting_depth = 0;
		}
		else {
			m_nesting_depth = m_superscope->m_nesting_depth + 1;
			m_superscope->m_subscope.push_back(this);
		}
	}

	~CScopeInfo()
	{
	}

	CScopeInfo<TSym> *Superscope()
	{
		return m_superscope;
	}

	void Add(TSym *symbol)
	{
		if (m_database.find(symbol->Label()) != m_database.end()) {
			throw CCompilationException(string("Symbol already exists: ") + symbol->Label());
		}
	
		m_database[symbol->Label()] = symbol;
	}

	TSym *GetOrDefault(const char *label)
	{
		// success
		if (m_database.find(label) != m_database.end()) {
			return m_database[label];
		}
	
		// failure
		if (m_superscope == NULL) {
			return NULL;
		}
	
		// keep searching
		return m_superscope->GetOrDefault(label);
	}

	TSym *Get(const char *label)
	{
		TSym *ret = GetOrDefault(label);
		if (ret == NULL) {
			throw CCompilationException(string("Symbol could not be found: ") + label);
		}
		return ret;
	}

	void Disp()
	{
		typename SymDB::iterator sym;
		for (sym = m_database.begin(); sym != m_database.end(); ++sym) {
			for (unsigned k = 0; k < m_nesting_depth; ++k) {
				dbgout << "\t";
			}
	
			dbgout << sym->first << "\n";
		}
	
		typename ScopeList::iterator subs;
		for (subs = m_subscope.begin(); subs != m_subscope.end(); ++subs) {
			dbgout << "\n";
			(*subs)->Disp();
		}
	}

private:
	typedef list<CScopeInfo<TSym> *> ScopeList;
	typedef map<string, TSym *> SymDB;

	CScopeInfo<TSym> *m_superscope;
	ScopeList m_subscope;
	SymDB m_database;
	unsigned m_nesting_depth;
};



class CSymbol : public LinkedListNode
{
public:
	enum TypeID {
		INTEGER,
		REAL,
		LABEL
	};

	CSymbol(const char *label, TypeID typeID);
	CSymbol(const char *label, const char *value, TypeID typeID);
	~CSymbol();

	string Label();
	TypeID Type();

	void SetValue(const char *value);
	string GetValue();
	bool ValueIsSet();
	void DiscardValue();

	void Patch(CSymbol *target);

	friend ostream& operator<<(ostream& os, const CSymbol& sym);

	static TypeID DiscoverType(const char *value);
	static const char *NumericToString(int value);
	static const char *NumericToString(float value);

private:
	enum ValueStatus {
		INVALID,
		VALID,
		CONST
	};

	inline CSymbol *Last() { return static_cast<CSymbol*>(LinkedListNode::Last()); }

	string m_label;
	TypeID m_typeID;
	ValueStatus m_valueStatus;
	string m_value;

	int m_memalloc;
	int m_regalloc;
};

CSymbol *newSymbol(const char *label, CSymbol::TypeID typeID);
CSymbol *newTemp(CSymbol::TypeID typeID);
CSymbol *newConst(const char *value);
CSymbol *newConst(int value);
CSymbol *newConst(float value);
CSymbol *newLabel(const char *prefix);



typedef CScopeInfo<CSymbol> SymbolScope;

#endif	// _SYMBOL_H

