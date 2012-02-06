#include <stdlib.h>
#include "semantic_rules.h"

#define NO_SYMBOL_SCOPING
SymbolScope *currentScope = NULL;
CCodeBlock emit;

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

