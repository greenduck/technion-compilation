
#include "utils.h"
#include "symbol.h"
#include "code.h"

/*
 * Symbol scoping
 */

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

static void ScopeInfo_unittest()
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
}

static void RXCode_unittest()
{
	CCodeBlock *cb = new CCodeBlock();

	CSymbol *a = new CSymbol("a", CSymbol::INTEGER);
	CSymbol *b = new CSymbol("b", CSymbol::INTEGER);

	cb->readi(a);
	cb->copyi(b, a);
	cb->prnti(b);
	cb->halt();

	for (CCodeBlock::iterator iter = cb->begin(); iter != cb->end(); ++iter) {
		cout << *iter << endl;
	}
}

int main()
{
	ScopeInfo_unittest();
	RXCode_unittest();
	return 0;
}

