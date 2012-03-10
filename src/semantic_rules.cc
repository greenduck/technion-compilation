#include <stdlib.h>
#include "semantic_rules.h"
#include "calc.h"

#define NO_SYMBOL_SCOPING
SymbolScope *currentScope = new SymbolScope(NULL);
// 'emit' object is a sintactic sugar referring to code emitting learned in compilation course
CCodeBlock emit;
CRegAlloc *regPool[2] = {
	/* INTEGER */ new CRegAlloc(NREGS, (const int[]){0, 1, 2, CRegAlloc::NOREG}),
	/* REAL    */ new CRegAlloc(NREGS)
};
CFuncSymbol *currentFunc = NULL;
CCodeBlock::SymDB constPropagationPending;

void program_init()
{
	// pre-allocate registers in global scope
	CSymbol *linkReg = regSymbol("@LR", CSymbol::INTEGER, 0);
	currentScope->Add(linkReg);

	CSymbol *framePtr = regSymbol("@FP", CSymbol::INTEGER, 1);
	currentScope->Add(framePtr);

	CSymbol *stackPtr = regSymbol("@SP", CSymbol::INTEGER, 2);
	currentScope->Add(stackPtr);
}

void main_init()
{
	// general initialization code
	emit.copy(currentScope->Get("@FP"), newConst(0));
	emit.copy(currentScope->Get("@SP"), newConst(0));
}

void blk_enter()
{
#ifndef NO_SYMBOL_SCOPING
	currentScope = new SymbolScope(currentScope);
#endif

	// code block of a function
	if (currentFunc != NULL) {
		// 1. insert function arguments into currentScope
		// 2. load function arguments from activation record
		int i;
		CFuncSymbol::iterator arg;
		for (i = 0, arg = currentFunc->begin(); arg != currentFunc->end(); ++i, ++arg) {
			currentScope->Add(*arg);
			emit.load_argument(*arg, i);
		}

		// function symbol is already in global symbol table
		currentFunc = NULL;
	}
}

void blk_exit()
{
#ifndef NO_SYMBOL_SCOPING
	SymbolScope *superscope = currentScope->Superscope();
	currentScope = superscope;
	BUG_IF((currentScope == NULL), "Unbalanced code block exit and entry");
#endif
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

	bool inverse_logic = emit.comp(op, cmp, p1, p2);
	if ( !inverse_logic ) {
		emit.breqz(cmp, falselabel);
		emit.ujump(truelabel);

		regPool[CSymbol::INTEGER]->Release(cmp->Register());

		return (SBpfac){ new CBPList(truelabel), new CBPList(falselabel) };
	}
	else {
		emit.bneqz(cmp, truelabel);
		emit.ujump(falselabel);

		regPool[CSymbol::INTEGER]->Release(cmp->Register());

		return (SBpfac){ new CBPList(falselabel), new CBPList(truelabel) };
	}
}

CSymbol *arithOp(const char *op, CSymbol *src0, CSymbol *src1)
{
	CSymbol *dest;
	bool performConstProp = constPropagation;

	if ( performConstProp ) {
		if (src0->ValueIsSet() && src1->ValueIsSet()) {
			// constant propagation
			string lvalue = calc(src0->GetValue() + " " + op + " " + src1->GetValue());
			dest = newConst(lvalue);
		}
		else {
			performConstProp = false;
		}
	}

	if ( !performConstProp ) {
		// code generation
		dest = newTemp(DestSymbolType(src0, src1));
		emit.arith(op, dest, src0, src1);
	}

	return dest;
}

void assnOp(CSymbol *dest, CSymbol *src)
{
	emit.copy(dest, src);

	if (constPropagation && src->ValueIsSet()) {
		dest->SetValue( src->GetValue() );
		constPropagationPending.push(dest);
	}
}

void StopConstPropagation()
{
	CSymbol *sym;

	dbgout << __FUNCTION__ << ": \n";
	while ( !constPropagationPending.empty() ) {
		sym = constPropagationPending.front();
		constPropagationPending.pop();
		if ( sym->ValueIsSet() ) {	dbgout << sym->Label() << " (" << sym->GetValue() << ") \n"; }
		sym->DiscardValue();
	}
}


void enforce_valid_nextlist(CBPList *&nextlist)
{
	if (nextlist == NULL) {
		CSymbol *next = newLabel("_____");
		emit.ujump(next);
		nextlist = new CBPList(next);
	}
}


/* cast if required
 * and possible without precision loss
 */
static CSymbol *CastIfRequired(CSymbol *p, CSymbol::TypeID typeID)
{
	CSymbol *ret;
	int flag = ((typeID == CSymbol::INTEGER) << 1) | (p->Type() == CSymbol::INTEGER);
	switch (flag)
	{
		case 3:	ret = p;
				break;
		case 2:	throw CCompilationException("Type mismatch between argument (" + p->Label() + ") and function signature");
		case 1:	ret = newTemp(CSymbol::REAL);
				emit.citor(ret, p);
				break;
		case 0:	ret = p;
				break;
	}
	return ret;
}

CSymbol *exp_call(CSymbol *p[2])
{
	CSymbol *fp = currentScope->Get("@FP");
	CFuncSymbol *fun = static_cast<CFuncSymbol*>(currentScope->Get("@function"));
	CSymbol *ret = newTemp(fun->Type());

	// TODO:
	// ideally, 'CastIfRequired' and 'push' should've been unified
	// in order to achieve better register utilization  
	int i;
	CFuncSymbol::iterator arg;
	for (i = 0, arg = fun->begin(); arg != fun->end(); ++i, ++arg) {
		BUG_IF((i > 2), "Number of arguments in function signature is greater than in function call");
		p[i] = CastIfRequired(p[i], (*arg)->Type());
	}

	// touch LR
	// In function prologue, we push all the registers that were touched.
	// If we perform function call, 'this' is not a leaf function, so LR should be stored.
	// In leaf function prologue LR is not stored.
	currentScope->regUsage[CSymbol::INTEGER]->Mirror( currentScope->Get("@LR")->Register() );

	emit.push(4, fp, NULL, p[0], p[1]);
	emit.jlink(fun);
	emit.pop(4, fp, ret, NULL, NULL);

	return ret;
}

SReturn return_exp(CSymbol *p)
{
	// TODO:
	// in case of compile-time errors
	// the error messages produced might be unclear because of referencing internal names  
	CFuncSymbol *fun = static_cast<CFuncSymbol*>(currentScope->Get("@function"));
	p = CastIfRequired(p, fun->Type());

	emit.store_retval(p);
	CSymbol *next = newLabel("_____");
	emit.ujump(next);
	return (SReturn){ new CBPList(next) };
}

/* Function Prologue:
 * Push all the registers touched by the function 
 * Return value:
 * a queue that contains all the registers pushed, needed for building Function Epilogue 
 * Note: 
 * as gathering register usage information requires function code parsing, 
 * the Prologue is emitted right before the marker label in the end of function processing
 */
CCodeBlock::SymDB function_prologue(CSymbol *prologue_label)
{
	CCodeBlock prologue;
	CCodeBlock::SymDB q;
	CSymbol *sym;

	// push ...
	// 1. Link Register (if this is not a leaf function) - already in 'regUsage'
	// 2. INTEGER registers
	// 3. REAL registers 
	for (int regID = currentScope->regUsage[CSymbol::INTEGER]->FirstAllocated(); regID != CRegAlloc::NOREG; regID = currentScope->regUsage[CSymbol::INTEGER]->NextAllocated(regID)) {
		sym = regSymbol("", CSymbol::INTEGER, regID);
		q.push(sym);
	}
	for (int regID = currentScope->regUsage[CSymbol::REAL]->FirstAllocated(); regID != CRegAlloc::NOREG; regID = currentScope->regUsage[CSymbol::REAL]->NextAllocated(regID)) {
		sym = regSymbol("", CSymbol::REAL, regID);
		q.push(sym);
	}

	CCodeBlock::SymDB qCopy(q);
	prologue.push(&q);

	// emit 'prologue'
	emit.Splice(prologue_label, prologue);

	return qCopy;
}

void function_epilogue(CCodeBlock::SymDB& q, CBPList *nextlist, CBPList *retlist)
{
	if (nextlist != NULL || retlist != NULL) {
		CSymbol *leave_label = newLabel("leave");
		emit.label(leave_label);

		if (nextlist != NULL)
			nextlist->Backpatch(leave_label);

		if (retlist != NULL)
			retlist->Backpatch(leave_label);
	}

	emit.pop(&q);
	emit.retrn();
}

