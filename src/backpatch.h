#ifndef _BACKPATCH_H
#define _BACKPATCH_H

#include <list>
#include "symbol.h"

class CBPList
{
public:
	CBPList(CSymbol *sym);
	~CBPList();

	CBPList *Merge(CSymbol *sym);
	CBPList *Merge(CBPList *bplist);
	void Backpatch(CSymbol *sym);

private:
	typedef list<CSymbol*> InternalList;

	InternalList m_list;
};

#endif	// _BACKPATCH_H

