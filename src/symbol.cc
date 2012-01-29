#include <iostream>

#include "symbol.h"
#include "utils.h"

template <typename TSym>
CScopeInfo<TSym>::CScopeInfo(CScopeInfo<TSym> *parent)
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

template <typename TSym>
CScopeInfo<TSym>::~CScopeInfo()
{
}

template <typename TSym>
void CScopeInfo<TSym>::Add(TSym *symbol)
{
	if (m_database.find(symbol->label) != m_database.end()) {
		throw CCompilationException(string("Symbol already exists: ") + symbol->label);
	}

	m_database[symbol->label] = symbol;
}

template <typename TSym>
TSym *CScopeInfo<TSym>::GetOrDefault(const char *label)
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

template <typename TSym>
TSym *CScopeInfo<TSym>::Get(const char *label)
{
	TSym *ret = GetOrDefault(label);
	if (ret == NULL) {
		throw CCompilationException(string("Symbol could not be found: ") + label);
	}
	return ret;
}

template <typename TSym>
void CScopeInfo<TSym>::dbg_disp()
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


class Symbol
{
public:
	string label;
	Symbol(const string name)
		:label(name)
	{
	}
	~Symbol()
	{
	}
};

int main()
{
	typedef CScopeInfo<Symbol> CScope;

	CScope *top;
	CScope *current;

	try {
		current = new CScope(NULL);
		top = current;
	
		Symbol a("a");
		Symbol b("b");
		Symbol c("c");
		current->Add(&a);
		current->Add(&b);
		current->Add(&c);
	
		current = new CScope(current);
		Symbol d("d");
		Symbol e("e");
		Symbol a2("a");
		current->Add(&d);
		current->Add(&e);
		current->Add(&a2);
		//current->Add(new Symbol("a"));

		top->dbg_disp();

		current->Get("a");
		current->Get("b");
		current->Get("x");
	}
	catch (CCompilationException ex) {
		cout << ex.String() << endl;
	}

	return 0;
}

