#include <string.h>
#include "code.h"
#include "utils.h"

static const char *opcode_to_string[] = {
	"COPYI",
	"PRNTI",
	"READI",
	"SEQUI",
	"SNEQI",
	"SLETI",
	"SGRTI",
	"ADD2I",
	"SUBTI",
	"MULTI",
	"DIVDI",
	"LOADI",
	"STORI",

	"COPYR",
	"PRNTR",
	"READR",
	"SEQUR",
	"SNEQR",
	"SLETR",
	"SGRTR",
	"ADD2R",
	"SUBTR",
	"MULTR",
	"DIVDR",
	"LOADR",
	"STORR",

	"CITOR",
	"CRTOI",

	"UJUMP",
	"JLINK",

	"RETRN",
	"BREQZ",
	"BNEQZ",

	"HALT"
};

Instruction::Instruction(OPCODE code, CSymbol *src0, CSymbol *src1, CSymbol *dest)
	:opcode(code),
	 args({src0, src1, dest})
{
}

ostream& operator<<(ostream& os, const Instruction& inst)
{
	os << opcode_to_string[inst.opcode];
	if (inst.args[DEST] != NULL) {
		os << " " << *inst.args[DEST];
	}
	for (int i = 0; i < NARGS - 1; ++i) {
		if (inst.args[i] != NULL) {
			os << " " << *inst.args[i];
		}
	}

	return os;
}



CCodeBlock::CCodeBlock()
{
}

CCodeBlock::~CCodeBlock()
{
}


void CCodeBlock::Disp()
{
	for (CCodeBlock::iterator it = begin(); it != end(); ++it) {
		dbgout << *it << "\n";
	}
}


void CCodeBlock::copy(CSymbol *dest, CSymbol *src)
{
	int flag = ((dest->Type() == CSymbol::INTEGER) << 1) | (src->Type() == CSymbol::INTEGER);
	switch (flag)
	{
	case 3:	copyi(dest, src);	break;
	case 2:	crtoi(dest, src);	break;
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
	CSymbol *c;

	if (dest->Type() != DestSymbolType(src0, src1))
		c = newTemp(CSymbol::REAL);
	else
		c = dest;

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
		case '*':	multi(c, a, b);	break;
		case '/':	divdi(c, a, b);	break;
		case '+':	add2i(c, a, b);	break;
		case '-':	subti(c, a, b);	break;
		}
		break;

	case 2:
	case 1:
	case 0:
		switch (op[0])
		{
		case '*':	multr(c, a, b);	break;
		case '/':	divdr(c, a, b);	break;
		case '+':	add2r(c, a, b);	break;
		case '-':	subtr(c, a, b);	break;
		}
		break;
	}

	if (dest->Type() != DestSymbolType(src0, src1))
		crtoi(dest, c);
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
	if (src->Type() == CSymbol::INTEGER)
		stori(src, addr0, addr1);
	else
		storr(src, addr0, addr1);
}


// TODO:
// Assert that each parameter has right type
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
	BUG_IF(true, "Non-implemented instruction");
}

void CCodeBlock::sneqi(CSymbol *dest, CSymbol *src0, CSymbol *src1)
{
	BUG_IF(true, "Non-implemented instruction");
}

void CCodeBlock::sleti(CSymbol *dest, CSymbol *src0, CSymbol *src1)
{
	BUG_IF(true, "Non-implemented instruction");
}

void CCodeBlock::sgrti(CSymbol *dest, CSymbol *src0, CSymbol *src1)
{
	BUG_IF(true, "Non-implemented instruction");
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
	BUG_IF(true, "Non-implemented instruction");
}

void CCodeBlock::stori(CSymbol *src, CSymbol *addr0, CSymbol *addr1)
{
	BUG_IF(true, "Non-implemented instruction");
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
	BUG_IF(true, "Non-implemented instruction");
}

void CCodeBlock::sneqr(CSymbol *dest, CSymbol *src0, CSymbol *src1)
{
	BUG_IF(true, "Non-implemented instruction");
}

void CCodeBlock::sletr(CSymbol *dest, CSymbol *src0, CSymbol *src1)
{
	BUG_IF(true, "Non-implemented instruction");
}

void CCodeBlock::sgrtr(CSymbol *dest, CSymbol *src0, CSymbol *src1)
{
	BUG_IF(true, "Non-implemented instruction");
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
	BUG_IF(true, "Non-implemented instruction");
}

void CCodeBlock::storr(CSymbol *src, CSymbol *addr0, CSymbol *addr1)
{
	BUG_IF(true, "Non-implemented instruction");
}


void CCodeBlock::citor(CSymbol *dest, CSymbol *src)
{
	m_codeDB.push_back(Instruction(CITOR, src, NULL, dest));
}

void CCodeBlock::crtoi(CSymbol *dest, CSymbol *src)
{
	m_codeDB.push_back(Instruction(CRTOI, src, NULL, dest));
}


void CCodeBlock::ujump(CSymbol *dest)
{
	BUG_IF(true, "Non-implemented instruction");
}

void CCodeBlock::jlink(CSymbol *dest)
{
	BUG_IF(true, "Non-implemented instruction");
}


void CCodeBlock::retrn(void)
{
	BUG_IF(true, "Non-implemented instruction");
}

void CCodeBlock::breqz(CSymbol *src, CSymbol *dest)
{
	BUG_IF(true, "Non-implemented instruction");
}

void CCodeBlock::bneqz(CSymbol *src, CSymbol *dest)
{
	BUG_IF(true, "Non-implemented instruction");
}


void CCodeBlock::halt(void)
{
	m_codeDB.push_back(Instruction(HALT, NULL, NULL, NULL));
}



CSymbol::TypeID DestSymbolType(CSymbol *src0, CSymbol *src1)
{
	if ((src0->Type() == CSymbol::INTEGER) && (src1->Type() == CSymbol::INTEGER))
		return CSymbol::INTEGER;

	return CSymbol::REAL;
}

