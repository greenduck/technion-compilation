#ifndef _SYMTAB_H
#define _SYMTAB_H

#include <map>
#include <list>
#include <string>

using namespace std;

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

#endif	// _SYMTAB_H

