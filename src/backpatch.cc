#include "backpatch.h"

CBPList::CBPList(CSymbol *sym)
{
	m_list.push_back(sym);
}

CBPList::~CBPList()
{
}

CBPList* CBPList::Merge(CSymbol *sym)
{
	m_list.push_back(sym);
	return this;
}

CBPList* CBPList::Merge(CBPList *bplist)
{
	m_list.sort();
	if (bplist != NULL) {
		bplist->m_list.sort();
		m_list.merge(bplist->m_list);
		m_list.unique();

		delete bplist;
	}

	return this;
}

void CBPList::Backpatch(CSymbol *sym)
{
	for (InternalList::iterator it = m_list.begin(); it != m_list.end(); ++it) {
		(*it)->Patch(sym);
	}

	delete this;
}

