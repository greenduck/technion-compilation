#include <stdlib.h>
#include "semantic_rules.h"

#define NO_SYMBOL_SCOPING
SymbolScope *currentScope = NULL;
CCodeBlock emit;
CRegAlloc *regPool[2] = {
	new CRegAlloc(1000, (const int[]){0, 1, 2}),	// INTEGER
	new CRegAlloc(1000)								// REAL
};

void blk_enter()
{
#ifdef NO_SYMBOL_SCOPING
	if (currentScope == NULL)
#endif
		currentScope = new SymbolScope(currentScope);
}

void blk_exit()
{
	SymbolScope *superscope = currentScope->Superscope();
	if (superscope != NULL) {
		currentScope = superscope;
	}
}

void dcl_applySymbols(IDQueue *symbols, CSymbol::TypeID typeID)
{
	while ( !symbols->empty() ) {
		char *label = symbols->front();
		symbols->pop();
		currentScope->Add(newSymbol(label, typeID));
		free(label);	// 'strndup()' in input_scanner.lex
	}
}

SBpfac bpfac_relOp(const char *op, CSymbol *p1, CSymbol *p2)
{
	CSymbol *cmp = newTemp(CSymbol::INTEGER);
	CSymbol *truelabel = newLabel("_____");
	CSymbol *falselabel = newLabel("_____");

	emit.comp(op, cmp, p1, p2);
	emit.breqz(cmp, falselabel);
	emit.ujump(truelabel);

	return (SBpfac){ new CBPList(truelabel), new CBPList(falselabel) };
}

