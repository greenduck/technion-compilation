#include "code.h"

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
}

void CCodeBlock::sneqi(CSymbol *dest, CSymbol *src0, CSymbol *src1)
{
}

void CCodeBlock::sleti(CSymbol *dest, CSymbol *src0, CSymbol *src1)
{
}

void CCodeBlock::sgrti(CSymbol *dest, CSymbol *src0, CSymbol *src1)
{
}

void CCodeBlock::add2i(CSymbol *dest, CSymbol *src0, CSymbol *src1)
{
}

void CCodeBlock::subti(CSymbol *dest, CSymbol *src0, CSymbol *src1)
{
}

void CCodeBlock::multi(CSymbol *dest, CSymbol *src0, CSymbol *src1)
{
}

void CCodeBlock::divdi(CSymbol *dest, CSymbol *src0, CSymbol *src1)
{
}

void CCodeBlock::loadi(CSymbol *dest, CSymbol *addr0, CSymbol *addr1)
{
}

void CCodeBlock::stori(CSymbol *src, CSymbol *addr0, CSymbol *addr1)
{
}


void CCodeBlock::copyr(CSymbol *dest, CSymbol *src)
{
}

void CCodeBlock::prntr(CSymbol *src)
{
}

void CCodeBlock::readr(CSymbol *dest)
{
}

void CCodeBlock::sequr(CSymbol *dest, CSymbol *src0, CSymbol *src1)
{
}

void CCodeBlock::sneqr(CSymbol *dest, CSymbol *src0, CSymbol *src1)
{
}

void CCodeBlock::sletr(CSymbol *dest, CSymbol *src0, CSymbol *src1)
{
}

void CCodeBlock::sgrtr(CSymbol *dest, CSymbol *src0, CSymbol *src1)
{
}

void CCodeBlock::add2r(CSymbol *dest, CSymbol *src0, CSymbol *src1)
{
}

void CCodeBlock::subtr(CSymbol *dest, CSymbol *src0, CSymbol *src1)
{
}

void CCodeBlock::multr(CSymbol *dest, CSymbol *src0, CSymbol *src1)
{
}

void CCodeBlock::divdr(CSymbol *dest, CSymbol *src0, CSymbol *src1)
{
}

void CCodeBlock::loadr(CSymbol *dest, CSymbol *addr0, CSymbol *addr1)
{
}

void CCodeBlock::storr(CSymbol *src, CSymbol *addr0, CSymbol *addr1)
{
}


void CCodeBlock::citor(CSymbol *dest, CSymbol *src)
{
}

void CCodeBlock::crtoi(CSymbol *dest, CSymbol *src)
{
}


void CCodeBlock::ujump(CSymbol *dest)
{
}

void CCodeBlock::jlink(CSymbol *dest)
{
}


void CCodeBlock::retrn(void)
{
}

void CCodeBlock::breqz(CSymbol *src, CSymbol *dest)
{
}

void CCodeBlock::bneqz(CSymbol *src, CSymbol *dest)
{
}


void CCodeBlock::halt(void)
{
	m_codeDB.push_back(Instruction(HALT, NULL, NULL, NULL));
}

