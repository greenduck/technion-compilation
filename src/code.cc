#include <string.h>
#include "code.h"
#include "utils.h"
#include "semantic_rules.h"

#ifdef X
#undef X
#endif
#define X(code)	#code,

static const char *opcode_to_string[] = {
	OPCODES
};

Instruction::Instruction(OPCODE code, CSymbol *src0, CSymbol *src1, CSymbol *dest)
	:opcode(code),
	 args({src0, src1, dest})
{
	if (code < LABEL)
		size = 1;
	else
		size = 0;
}

ostream& operator<<(ostream& os, const Instruction& inst)
{
	if (inst.opcode < LABEL) {
		os << opcode_to_string[inst.opcode];
		if (inst.args[DEST] != NULL) {
			os << " " << *inst.args[DEST];
		}
		for (int i = 0; i < NARGS - 1; ++i) {
			if (inst.args[i] != NULL) {
				os << " " << *inst.args[i];
			}
		}
	}
	else {
		os << *inst.args[SRC0] << ":";
	}

	return os;
}



CCodeBlock::CCodeBlock()
{
}

CCodeBlock::~CCodeBlock()
{
}


void CCodeBlock::Splice(CSymbol *marker, CCodeBlock& subBlock)
{
	for (iterator it = begin(); it != end(); ++it) {
		if (it->args[SRC0] == marker) {
			// marker symbol found ...
			m_codeDB.splice(it, subBlock.m_codeDB);
			return;
		}
	}

	BUG_IF(true, "Could not identify splice marker");
}


void CCodeBlock::Disp(ostream& os)
{
	for (CCodeBlock::iterator inst = begin(); inst != end(); ++inst) {
		if ((os == dbgout) || (inst->size > 0)) {
			os << *inst << "\n";
		}
	}
}


int CCodeBlock::CalcLabelAddress(int startAddr)
{
	int currentAddr = startAddr;
	for (CCodeBlock::iterator inst = begin(); inst != end(); ++inst) {
		if (inst->opcode == LABEL) {
			CSymbol *label = inst->args[SRC0];
			label->SetValue(CSymbol::NumericToString(currentAddr));
		}

		currentAddr += inst->size;
	}

	return currentAddr;
}



void CCodeBlock::copy(CSymbol *dest, CSymbol *src)
{
	int flag = ((dest->Type() == CSymbol::INTEGER) << 1) | (src->Type() == CSymbol::INTEGER);
	switch (flag)
	{
	case 3:	copyi(dest, src);	break;
	case 2:	throw CCompilationException("Type mismatch between lvalue (" + dest->Label() + ") and rvalue (" + src->Label() + ")");
	case 1:	citor(dest, src);	break;
	case 0:	copyr(dest, src);	break;
	}
}

void CCodeBlock::prnt(CSymbol *src)
{
	if (src->Type() == CSymbol::INTEGER)
		prnti(src);
	else
		prntr(src);
}

void CCodeBlock::read(CSymbol *dest)
{
	if (dest->Type() == CSymbol::INTEGER)
		readi(dest);
	else
		readr(dest);
}

void CCodeBlock::comp(const char *op, CSymbol *dest, CSymbol *src0, CSymbol *src1)
{
	CSymbol *a;
	CSymbol *b;
	int flag = ((src0->Type() == CSymbol::INTEGER) << 1) | (src1->Type() == CSymbol::INTEGER);
	switch (flag)
	{
	case 3:	a = src0;
			b = src1;
			break;
	case 2:	a = newTemp(CSymbol::REAL);
			citor(a, src0);
			b = src1;
			break;
	case 1:	a = src0;
			b = newTemp(CSymbol::REAL);
			citor(b, src1);
			break;
	case 0:	a = src0;
			b = src1;
			break;
	}

	switch (flag)
	{
	case 3:	if (!strcmp("==", op))			sequi(dest, a, b);
			else if (!strcmp("<>", op))		sneqi(dest, a, b);
			else if (!strcmp("<",  op))		sleti(dest, a, b);
			else if (!strcmp("<=", op))		sgrti(dest, b, a);
			else if (!strcmp(">",  op))		sgrti(dest, a, b);
			else if (!strcmp(">=", op))		sleti(dest, b, a);
			break;

	case 2:
	case 1:
	case 0: if (!strcmp("==", op))			sequr(dest, a, b);
			else if (!strcmp("<>", op))		sneqr(dest, a, b);
			else if (!strcmp("<",  op))		sletr(dest, a, b);
			else if (!strcmp("<=", op))		sgrtr(dest, b, a);
			else if (!strcmp(">",  op))		sgrtr(dest, a, b);
			else if (!strcmp(">=", op))		sletr(dest, b, a);
			break;
	}
}

void CCodeBlock::arith(const char *op, CSymbol *dest, CSymbol *src0, CSymbol *src1)
{
	CSymbol *a;
	CSymbol *b;

	BUG_IF((dest->Type() != DestSymbolType(src0, src1)), "Type mismatch between destination and source in arithmetic operation");

	int flag = ((src0->Type() == CSymbol::INTEGER) << 1) | (src1->Type() == CSymbol::INTEGER);
	switch (flag)
	{
	case 3:	a = src0;
			b = src1;
			break;
	case 2:	a = newTemp(CSymbol::REAL);
			citor(a, src0);
			b = src1;
			break;
	case 1:	a = src0;
			b = newTemp(CSymbol::REAL);
			citor(b, src1);
			break;
	case 0:	a = src0;
			b = src1;
			break;
	}

	switch (flag)
	{
	case 3:
		switch (op[0])
		{
		case '*':	multi(dest, a, b);	break;
		case '/':	divdi(dest, a, b);	break;
		case '+':	add2i(dest, a, b);	break;
		case '-':	subti(dest, a, b);	break;
		}
		break;

	case 2:
	case 1:
	case 0:
		switch (op[0])
		{
		case '*':	multr(dest, a, b);	break;
		case '/':	divdr(dest, a, b);	break;
		case '+':	add2r(dest, a, b);	break;
		case '-':	subtr(dest, a, b);	break;
		}
		break;
	}
}

void CCodeBlock::load(CSymbol *dest, CSymbol *addr0, CSymbol *addr1)
{
	if (dest->Type() == CSymbol::INTEGER)
		loadi(dest, addr0, addr1);
	else
		loadr(dest, addr0, addr1);
}

void CCodeBlock::stor(CSymbol *src, CSymbol *addr0, CSymbol *addr1)
{
	CSymbol *a;

	if (src->Type() == CSymbol::INTEGER) {
		if (src->ValueIsSet()) {
			a = newTemp(CSymbol::INTEGER);
			copyi(a, src);
			stori(a, addr0, addr1);
			regPool[CSymbol::INTEGER]->Release(a->Register());
		}
		else {
			stori(src, addr0, addr1);
		}
	}
	else {
		if (src->ValueIsSet()) {
			a = newTemp(CSymbol::REAL);
			copyr(a, src);
			storr(a, addr0, addr1);
			regPool[CSymbol::REAL]->Release(a->Register());
		}
		else {
			storr(src, addr0, addr1);
		}
	}
}

void CCodeBlock::push(int argc, ...)
{
	va_list args;
	CSymbol *sym;
	SymDB q;

	va_start(args, argc);
	for (int i = 0; i < argc; ++i) {
		sym = va_arg(args, CSymbol*);
		q.push(sym);
	}
	va_end(args);

	push(&q);
}

void CCodeBlock::pop(int argc, ...)
{
	va_list args;
	CSymbol *sym;
	SymDB q;

	va_start(args, argc);
	for (int i = 0; i < argc; ++i) {
		sym = va_arg(args, CSymbol*);
		q.push(sym);
	}
	va_end(args);

	pop(&q);
}

void CCodeBlock::push(SymDB *q)
{
	CSymbol *sym;
	CSymbol *offset;
	CSymbol *sp = currentScope->Get("@SP");
	int argc = q->size();

	for (int i = 0; i < argc; ++i) {
		sym = q->front();
		q->pop();

		if (sym != NULL) {
			offset = newConst(i);
			stor(sym, sp, offset);

			// each time FP is pushed,
			// FP catches up-to-date value of SP
			if (sym->Label() == "@FP") {
				arith("+", sym, sp, newConst(i + 1));
			}
		}
	}

	arith("+", sp, sp, newConst(argc));
}

void CCodeBlock::pop(SymDB *q)
{
	CSymbol *sym;
	CSymbol *offset;
	CSymbol *sp = currentScope->Get("@SP");
	int argc = q->size();

	arith("-", sp, sp, newConst(argc));

	for (int i = 0; i < argc; ++i) {
		sym = q->front();
		q->pop();

		if (sym != NULL) {
			offset = newConst(i);
			load(sym, sp, offset);
		}
	}
}

void CCodeBlock::load_argument(CSymbol *dest, int index)
{
	emit.load(dest, currentScope->Get("@FP"), newConst(index + 1));
}

void CCodeBlock::store_retval(CSymbol *src)
{
	emit.stor(src, currentScope->Get("@FP"), newConst(0));
}

/* ----- */
void CCodeBlock::copyi(CSymbol *dest, CSymbol *src)
{
	
	m_codeDB.push_back(Instruction(COPYI, src, NULL, dest));
}

void CCodeBlock::prnti(CSymbol *src)
{
	m_codeDB.push_back(Instruction(PRNTI, src, NULL, NULL));
}

void CCodeBlock::readi(CSymbol *dest)
{
	m_codeDB.push_back(Instruction(READI, NULL, NULL, dest));
}

void CCodeBlock::sequi(CSymbol *dest, CSymbol *src0, CSymbol *src1)
{
	m_codeDB.push_back(Instruction(SEQUI, src0, src1, dest));
}

void CCodeBlock::sneqi(CSymbol *dest, CSymbol *src0, CSymbol *src1)
{
	m_codeDB.push_back(Instruction(SNEQI, src0, src1, dest));
}

void CCodeBlock::sleti(CSymbol *dest, CSymbol *src0, CSymbol *src1)
{
	m_codeDB.push_back(Instruction(SLETI, src0, src1, dest));
}

void CCodeBlock::sgrti(CSymbol *dest, CSymbol *src0, CSymbol *src1)
{
	m_codeDB.push_back(Instruction(SGRTI, src0, src1, dest));
}

void CCodeBlock::add2i(CSymbol *dest, CSymbol *src0, CSymbol *src1)
{
	m_codeDB.push_back(Instruction(ADD2I, src0, src1, dest));
}

void CCodeBlock::subti(CSymbol *dest, CSymbol *src0, CSymbol *src1)
{
	m_codeDB.push_back(Instruction(SUBTI, src0, src1, dest));
}

void CCodeBlock::multi(CSymbol *dest, CSymbol *src0, CSymbol *src1)
{
	m_codeDB.push_back(Instruction(MULTI, src0, src1, dest));
}

void CCodeBlock::divdi(CSymbol *dest, CSymbol *src0, CSymbol *src1)
{
	m_codeDB.push_back(Instruction(MULTI, src0, src1, dest));
}

void CCodeBlock::loadi(CSymbol *dest, CSymbol *addr0, CSymbol *addr1)
{
	m_codeDB.push_back(Instruction(LOADI, addr0, addr1, dest));
}

void CCodeBlock::stori(CSymbol *src, CSymbol *addr0, CSymbol *addr1)
{
	m_codeDB.push_back(Instruction(STORI, addr0, addr1, src));
}


void CCodeBlock::copyr(CSymbol *dest, CSymbol *src)
{
	m_codeDB.push_back(Instruction(COPYR, src, NULL, dest));
}

void CCodeBlock::prntr(CSymbol *src)
{
	m_codeDB.push_back(Instruction(PRNTR, src, NULL, NULL));
}

void CCodeBlock::readr(CSymbol *dest)
{
	m_codeDB.push_back(Instruction(PRNTR, NULL, NULL, dest));
}

void CCodeBlock::sequr(CSymbol *dest, CSymbol *src0, CSymbol *src1)
{
	m_codeDB.push_back(Instruction(SEQUR, src0, src1, dest));
}

void CCodeBlock::sneqr(CSymbol *dest, CSymbol *src0, CSymbol *src1)
{
	m_codeDB.push_back(Instruction(SNEQR, src0, src1, dest));
}

void CCodeBlock::sletr(CSymbol *dest, CSymbol *src0, CSymbol *src1)
{
	m_codeDB.push_back(Instruction(SLETR, src0, src1, dest));
}

void CCodeBlock::sgrtr(CSymbol *dest, CSymbol *src0, CSymbol *src1)
{
	m_codeDB.push_back(Instruction(SGRTR, src0, src1, dest));
}

void CCodeBlock::add2r(CSymbol *dest, CSymbol *src0, CSymbol *src1)
{
	m_codeDB.push_back(Instruction(ADD2R, src0, src1, dest));
}

void CCodeBlock::subtr(CSymbol *dest, CSymbol *src0, CSymbol *src1)
{
	m_codeDB.push_back(Instruction(SUBTR, src0, src1, dest));
}

void CCodeBlock::multr(CSymbol *dest, CSymbol *src0, CSymbol *src1)
{
	m_codeDB.push_back(Instruction(MULTR, src0, src1, dest));
}

void CCodeBlock::divdr(CSymbol *dest, CSymbol *src0, CSymbol *src1)
{
	m_codeDB.push_back(Instruction(DIVDR, src0, src1, dest));
}

void CCodeBlock::loadr(CSymbol *dest, CSymbol *addr0, CSymbol *addr1)
{
	m_codeDB.push_back(Instruction(LOADR, addr0, addr1, dest));
}

void CCodeBlock::storr(CSymbol *src, CSymbol *addr0, CSymbol *addr1)
{
	m_codeDB.push_back(Instruction(STORR, addr0, addr1, src));
}


void CCodeBlock::citor(CSymbol *dest, CSymbol *src)
{
	m_codeDB.push_back(Instruction(CITOR, src, NULL, dest));
}

void CCodeBlock::crtoi(CSymbol *dest, CSymbol *src)
{
	m_codeDB.push_back(Instruction(CRTOI, src, NULL, dest));
}


void CCodeBlock::ujump(CSymbol *src)
{
	m_codeDB.push_back(Instruction(UJUMP, src, NULL, NULL));
}

void CCodeBlock::jlink(CSymbol *src)
{
	m_codeDB.push_back(Instruction(JLINK, src, NULL, NULL));
}


void CCodeBlock::retrn(void)
{
	m_codeDB.push_back(Instruction(RETRN, NULL, NULL, NULL));
}

void CCodeBlock::breqz(CSymbol *src0, CSymbol *src1)
{
	m_codeDB.push_back(Instruction(BREQZ, src0, src1, NULL));
}

void CCodeBlock::bneqz(CSymbol *src0, CSymbol *src1)
{
	m_codeDB.push_back(Instruction(BNEQZ, src0, src1, NULL));
}


void CCodeBlock::halt(void)
{
	m_codeDB.push_back(Instruction(HALT, NULL, NULL, NULL));
}


void CCodeBlock::label(CSymbol *src)
{
	m_codeDB.push_back(Instruction(LABEL, src, NULL, NULL));
}



CSymbol::TypeID DestSymbolType(CSymbol *src0, CSymbol *src1)
{
	if ((src0->Type() == CSymbol::INTEGER) && (src1->Type() == CSymbol::INTEGER))
		return CSymbol::INTEGER;

	return CSymbol::REAL;
}

