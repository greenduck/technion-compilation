#ifndef _SYMBOL_H
#define _SYMBOL_H

#include <map>
#include <list>
#include <string>

#include "utils.h"

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

	void Add(TSym *symbol)
	{
		if (m_database.find(symbol->label) != m_database.end()) {
			throw CCompilationException(string("Symbol already exists: ") + symbol->label);
		}
	
		m_database[symbol->label] = symbol;
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

	void dbg_disp()
	{
		typename SymDB::iterator sym;
		for (sym = m_database.begin(); sym != m_database.end(); ++sym) {
			for (unsigned k = 0; k < m_nesting_depth; ++k) {
				cout << "\t";
			}
	
			cout << sym->first << endl;
		}
	
		typename ScopeList::iterator subs;
		for (subs = m_subscope.begin(); subs != m_subscope.end(); ++subs) {
			(*subs)->dbg_disp();
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



class CSymbol
{
public:
	enum TypeID {
		INTEGER,
		REAL
	};

	CSymbol(const char *label, TypeID typeID);
//	CSymbol(const char *value, TypeID typeID);
	~CSymbol();

	string Label();
	TypeID Type();

	void SetValue(const char *value);
	string GetValue();
	bool ValueIsSet();
	void DiscardValue();

	friend ostream& operator<<(ostream& os, const CSymbol& sym);

private:
	enum ValueStatus {
		INVALID,
		VALID,
		CONST
	};

	string m_label;
	TypeID m_typeID;
	ValueStatus m_valueStatus;
	string m_value;

	int m_memalloc;
	int m_regalloc;
};

#endif	// _SYMBOL_H

