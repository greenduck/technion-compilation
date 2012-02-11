#ifndef _SEMANTIC_RULES_H
#define _SEMANTIC_RULES_H

#include <queue>
#include "symbol.h"
#include "code.h"
#include "backpatch.h"

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
	CBPList *nextlist;
};

struct SList {
	CBPList *nextlist;
};

struct SBlk {
	CBPList *nextlist;
};

struct SBpfac {
	CBPList *truelist;
	CBPList *falselist;
};

struct SBfac {
	CBPList *truelist;
	CBPList *falselist;
};

struct SBterm {
	CBPList *truelist;
	CBPList *falselist;
};

struct SBexp {
	CBPList *truelist;
	CBPList *falselist;
};

struct SCntrl {
	CBPList *nextlist;
};

struct SM1 {
	CSymbol *label;
};

extern SymbolScope *currentScope;
extern CCodeBlock emit;

void blk_enter();
void blk_exit();
void dcl_applySymbols(IDQueue *symbols, CSymbol::TypeID typeID);
SBpfac bpfac_relOp(const char *op, CSymbol *p1, CSymbol *p2);

#endif	// _SEMANTIC_RULES_H

