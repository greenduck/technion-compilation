#ifndef _SEMANTIC_RULES_H
#define _SEMANTIC_RULES_H

#include <queue>
#include <list>
#include "symbol.h"
#include "code.h"
#include "backpatch.h"
#include "storage.h"

#define NREGS	1000

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
	CBPList *retlist;
};

struct SList {
	CBPList *nextlist;
	CBPList *retlist;
};

struct SBlk {
	CBPList *nextlist;
	CBPList *retlist;
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
	CBPList *retlist;
};

struct SReturn {
	CBPList *retlist;
};

struct SM1 {
	CSymbol *label;
};

struct SM2 {
	CSymbol *skip_label;
	CSymbol *prologue_label;
};

struct SM5 {
	CBPList *dest;
};

extern SymbolScope *currentScope;
extern CCodeBlock emit;
extern CRegAlloc *regPool[2];
extern CFuncSymbol *currentFunc;

void program_init();
void main_init();
void blk_enter();
void blk_exit();
void dcl_applySymbols(IDQueue *symbols, CSymbol::TypeID typeID);
SBpfac bpfac_relOp(const char *op, CSymbol *p1, CSymbol *p2);

void enforce_valid_nextlist(CBPList *&nextlist);

CSymbol *exp_call(CSymbol *p[2]);
SReturn return_exp(CSymbol *p);
CCodeBlock::SymDB function_prologue(CSymbol *prologue_label);
void function_epilogue(CCodeBlock::SymDB& q, CBPList *nextlist, CBPList *retlist);

#endif	// _SEMANTIC_RULES_H

