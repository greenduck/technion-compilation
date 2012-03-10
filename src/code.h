/* 
 * Basic code definitions: 
 * Instruction - a single instruction encoding 
 * CCodeBlock - a single block of instructions, implementing each instruction emitting 
 */
#ifndef _CODE_H
#define _CODE_H

#include <cstdarg>
#include <iostream>
#include <list>
#include <queue>

#include "symbol.h"

#define OPCODES	\
	X(COPYI)	\
	X(PRNTI)	\
	X(READI)	\
	X(SEQUI)	\
	X(SNEQI)	\
	X(SLETI)	\
	X(SGRTI)	\
	X(ADD2I)	\
	X(SUBTI)	\
	X(MULTI)	\
	X(DIVDI)	\
	X(LOADI)	\
	X(STORI)	\
				\
	X(COPYR)	\
	X(PRNTR)	\
	X(READR)	\
	X(SEQUR)	\
	X(SNEQR)	\
	X(SLETR)	\
	X(SGRTR)	\
	X(ADD2R)	\
	X(SUBTR)	\
	X(MULTR)	\
	X(DIVDR)	\
	X(LOADR)	\
	X(STORR)	\
				\
	X(CITOR)	\
	X(CRTOI)	\
				\
	X(UJUMP)	\
	X(JLINK)	\
				\
	X(RETRN)	\
	X(BREQZ)	\
	X(BNEQZ)	\
				\
	X(HALT)		\
				\
	X(LABEL)		/* merely a placeholder */

#ifdef X
#undef X
#endif
#define X(code)	code,

enum OPCODE {
	OPCODES
};

enum INSTARGS {
	SRC0 = 0,
	SRC1,
	DEST,

	NARGS
};

struct Instruction
{
	OPCODE opcode;
	CSymbol *args[NARGS];
	int size;

	Instruction(OPCODE code, CSymbol *src0, CSymbol *src1, CSymbol *dest);
	friend ostream& operator<<(ostream& os, const Instruction& cl);
};

class CCodeBlock
{
private:
	typedef list<Instruction> CodeDB;

	CodeDB m_codeDB;

public:
	typedef queue<CSymbol*> SymDB;

	CCodeBlock();
	~CCodeBlock();

    // interface to list::iterator
    typedef CodeDB::iterator iterator;
    inline iterator begin() { return m_codeDB.begin(); }
    inline iterator end() { return m_codeDB.end(); }

	void Splice(CSymbol *marker, CCodeBlock& subBlock);
	void Disp(ostream& os = dbgout);
	int CalcLabelAddress(int startAddr);

	// instruction implementation
	// convenience macro instructions
	void copy(CSymbol *dest, CSymbol *src);
	void prnt(CSymbol *src);
	void read(CSymbol *dest);
	bool comp(const char *op, CSymbol *dest, CSymbol *src0, CSymbol *src1);
	void arith(const char *op, CSymbol *dest, CSymbol *src0, CSymbol *src1);
	void load(CSymbol *dest, CSymbol *addr0, CSymbol *addr1);
	void stor(CSymbol *src, CSymbol *addr0, CSymbol *addr1);
	void push(int argc, ...);
	void pop(int argc, ...);
	void push(SymDB *q);
	void pop(SymDB *q);
	void load_argument(CSymbol *dest, int index);
	void store_retval(CSymbol *src);

	// machine instructions
	void copyi(CSymbol *dest, CSymbol *src);
	void prnti(CSymbol *src);
	void readi(CSymbol *dest);
	void sequi(CSymbol *dest, CSymbol *src0, CSymbol *src1);
	void sneqi(CSymbol *dest, CSymbol *src0, CSymbol *src1);
	void sleti(CSymbol *dest, CSymbol *src0, CSymbol *src1);
	void sgrti(CSymbol *dest, CSymbol *src0, CSymbol *src1);
	void add2i(CSymbol *dest, CSymbol *src0, CSymbol *src1);
	void subti(CSymbol *dest, CSymbol *src0, CSymbol *src1);
	void multi(CSymbol *dest, CSymbol *src0, CSymbol *src1);
	void divdi(CSymbol *dest, CSymbol *src0, CSymbol *src1);
	void loadi(CSymbol *dest, CSymbol *addr0, CSymbol *addr1);
	void stori(CSymbol *src, CSymbol *addr0, CSymbol *addr1);

	void copyr(CSymbol *dest, CSymbol *src);
	void prntr(CSymbol *src);
	void readr(CSymbol *dest);
	void sequr(CSymbol *dest, CSymbol *src0, CSymbol *src1);
	void sneqr(CSymbol *dest, CSymbol *src0, CSymbol *src1);
	void sletr(CSymbol *dest, CSymbol *src0, CSymbol *src1);
	void sgrtr(CSymbol *dest, CSymbol *src0, CSymbol *src1);
	void add2r(CSymbol *dest, CSymbol *src0, CSymbol *src1);
	void subtr(CSymbol *dest, CSymbol *src0, CSymbol *src1);
	void multr(CSymbol *dest, CSymbol *src0, CSymbol *src1);
	void divdr(CSymbol *dest, CSymbol *src0, CSymbol *src1);
	void loadr(CSymbol *dest, CSymbol *addr0, CSymbol *addr1);
	void storr(CSymbol *src, CSymbol *addr0, CSymbol *addr1);

	void citor(CSymbol *dest, CSymbol *src);
	void crtoi(CSymbol *dest, CSymbol *src);

	void ujump(CSymbol *src);
	void jlink(CSymbol *src);

	void retrn(void);
	void breqz(CSymbol *src0, CSymbol *src1);
	void bneqz(CSymbol *src0, CSymbol *src1);

	void halt(void);

	void label(CSymbol *src);
};



CSymbol::TypeID DestSymbolType(CSymbol *src0, CSymbol *src1);

#endif	// _CODE_H

