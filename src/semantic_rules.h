#ifndef _SEMANTIC_RULES_H
#define _SEMANTIC_RULES_H

#include <queue>
#include "symbol.h"
#include "code.h"

using namespace std;

typedef queue<char*> IDQueue;

struct SFactor {
	CSymbol *place;
};

struct STerm {
	CSymbol *place;
};

struct SExp {
	CSymbol *place;
};

struct SStmt {
	void *nextlist;
};

struct SList {
	void *nextlist;
};

struct SBlk {
	void *nextlist;
};

extern SymbolScope *currentScope;
extern CCodeBlock emit;

void blk_enter();
void blk_exit();
void dcl_applySymbols(IDQueue *symbols, CSymbol::TypeID typeID);

#endif	// _SEMANTIC_RULES_H

