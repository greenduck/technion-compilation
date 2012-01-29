#ifndef _SYMBOL_H
#define _SYMBOL_H

#include <map>
#include <list>
#include <string>

using namespace std;

// data structure for handling nested scopes
template <typename TSym>
class CScopeInfo
{
public:
	CScopeInfo(CScopeInfo<TSym> *parent);
	~CScopeInfo();

	void Add(TSym *symbol);
	TSym *GetOrDefault(const char *label);
	TSym *Get(const char *label);
	void dbg_disp();

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
	CSymbol(T value, TypeID typeID);
	~CSymbol();

	const string Label();
	TypeID Type();

	void SetValue(T value);
	T GetValue();
	bool ValueIsSet();
	void DiscardValue();

	// debug
	string String();

private:
	enum ValueStatus {
		INVALID,
		VALID,
		CONST
	};

	string m_label;
	TypeID m_typeID;
	ValueStatus m_valueStatus;
	T m_value;

	int m_memalloc;
	int m_regalloc;
};

#endif	// _SYMBOL_H

