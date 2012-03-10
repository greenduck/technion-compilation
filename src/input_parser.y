
	/* definitions */
%{

#include <stdio.h>
#include "utils.h"
#include "semantic_rules.h"
#include "symbol.h"

// a patch that prevents emitting excessive marker labels
static bool stmt_has_nextlist = false;

%}

%union {
	char		*text;
	CSymbol::TypeID	typeID;
	IDQueue		*idQueue;
	SFactor		_factor;
	STerm		_term;
	SExp		_exp;
	SStmt		_stmt;
	SList		_list;
	SBlk		_blk;
	SBpfac		_bpfac;
	SBfac		_bfac;
	SBterm		_bterm;
	SBexp		_bexp;
	SCntrl		_cntrl;
	SReturn		_return;
	SM1		_m1;
	SM2		_m2;
	SM5		_m5;
}

%token PROGRAM
%token LPAREN
%token RPAREN
%token COLON
%token COMMA
%token FUNCTION
%token BEGIN_
%token END
%token VAR
%token SEMICOLON
%token INTEGER
%token REAL
%token WRITE
%token READ
%token RETURN
%token ASSIGN
%token IF
%token THEN
%token ELSE
%token FOR
%token WHILE
%token DO
%token OR
%token AND
%token NOT
%token CALL

%token <text> NUM
%token <text> ID
%token <text> ADDOP
%token <text> MULOP
%token <text> RELOP

%start		program
// %type <node>	function
// %type <node>	main_function
%type <_blk>	blk
// %type <typeID>	func_ret
// %type <symList>	func_arg
%type <typeID>	type
// %type <node>	declarations
%type <_list>	list
// %type <node>	declarlist
// %type <node>	dcl
%type <idQueue>	idents
%type <_stmt>	stmt
// %type <node>	assn
%type <_cntrl>	cntrl
// %type <node>	read
// %type <node>	write
%type <_return>	return
%type <_exp>	exp
%type <_bexp>	bexp
%type <_bterm>	bterm
%type <_bfac>	bfac
%type <_bpfac>	bpfac
%type <_term>	term
%type <_factor>	factor

%type <_m1>	m1
%type <_m2>	m2
%type <_m5>	m5
%type <_m1>	m6

%right IF THEN ELSE

%%
	/* semantic rules */

program		: m3 function main_function		{ dbgout << "Syntax Ok \n";
							  return 0;
							}
		;

main_function	: m4 PROGRAM blk			{ if ($3.retlist != NULL)
								throw CCompilationException("'return' statement used (somewhere) in code block that does not belong to a function");
							  if ($3.nextlist != NULL) {
								CSymbol *label = newLabel("halt");
								emit.label(label);
								$3.nextlist->Backpatch(label);
							  }
							  emit.halt();
							}
		;

function	: m2 func_ret func_arg blk		{ CCodeBlock::SymDB q = function_prologue($1.prologue_label);
							  function_epilogue(q, $4.nextlist, $4.retlist);
							  emit.label($1.skip_label);
							}
		| /* eps */				{ }
		;

func_arg	: LPAREN ID COLON type COMMA ID COLON type RPAREN	{
						    	  BUG_IF((currentFunc == NULL), "Expected an existing function");
							  currentFunc->AddArgument(newSymbol($2, $4));
							  currentFunc->AddArgument(newSymbol($6, $8));
							  currentScope->Add(currentFunc);
							}
		;

func_ret	: type FUNCTION				{ BUG_IF((currentFunc != NULL), "Expected to start all new function");
							  currentFunc = new CFuncSymbol("@function", $1);
							}
		;

blk		: m0 declarations BEGIN_ list END	{ $$.nextlist = $4.nextlist;
							  $$.retlist = $4.retlist;
							  blk_exit();
							}
		;

declarations	: VAR declarlist			{ }
		| /* eps */				{ }
		;

declarlist	: dcl declarlist			{ }
		| /* eps */				{ }
		;

dcl		: idents COLON type SEMICOLON		{ dcl_applySymbols($1, $3);
							  delete $1;
							}
		;

idents		: ID					{ $$ = new IDQueue();
							  $$->push($1);
							}
		| idents COMMA ID			{ $$ = $1;
							  $$->push($3);
							}
		;

type		: INTEGER				{ $$ = CSymbol::INTEGER; }
		| REAL					{ $$ = CSymbol::REAL; }
		;

list		: list m6 stmt				{ if ($1.nextlist != NULL)
								$1.nextlist->Backpatch($2.label);
							  $$.nextlist = $3.nextlist;
							  $$.retlist = ($1.retlist != NULL) ? $1.retlist->Merge($3.retlist) : $3.retlist;
							}
		| /* eps */				{ $$.nextlist = NULL; }
		;

stmt		: assn					{ $$.nextlist = NULL;
							  $$.retlist = NULL;
							  stmt_has_nextlist = ($$.nextlist != NULL);
							}
		| cntrl					{ $$.nextlist = $1.nextlist;
							  $$.retlist = $1.retlist;
							  stmt_has_nextlist = ($$.nextlist != NULL);
							}
		| read					{ $$.nextlist = NULL;
							  $$.retlist = NULL;
							  stmt_has_nextlist = ($$.nextlist != NULL);
							}
		| write					{ $$.nextlist = NULL;
							  $$.retlist = NULL;
							  stmt_has_nextlist = ($$.nextlist != NULL);
							}
		| return				{ $$.nextlist = NULL;
							  $$.retlist = $1.retlist;
							  stmt_has_nextlist = ($$.nextlist != NULL);
							}
		| blk					{ $$.nextlist = $1.nextlist;
							  $$.retlist = $1.retlist;
							  stmt_has_nextlist = ($$.nextlist != NULL);
							}
		;

write		: WRITE LPAREN exp RPAREN SEMICOLON	{ emit.prnt($3.place); }
		;

return		: RETURN LPAREN exp RPAREN SEMICOLON	{ $$ = return_exp($3.place); }
		;

read		: READ LPAREN ID RPAREN SEMICOLON	{ emit.read(currentScope->Get($3)); }
		;

assn		: ID ASSIGN exp SEMICOLON		{ assnOp(currentScope->Get($1), $3.place); }
		;

cntrl		: IF bexp THEN m1 stmt ELSE m5 m1 stmt	{ $2.truelist->Backpatch($4.label);
							  $2.falselist->Backpatch($8.label);
							  enforce_valid_nextlist($9.nextlist);
							  $$.nextlist = $7.dest->Merge($5.nextlist)->Merge($9.nextlist);
							  $$.retlist = ($5.retlist != NULL) ? $5.retlist->Merge($9.retlist) : $9.retlist;
							}

		| IF bexp THEN m1 stmt			{ $2.truelist->Backpatch($4.label);
							  enforce_valid_nextlist($5.nextlist);
							  $$.nextlist = $2.falselist->Merge($5.nextlist);
							  $$.retlist = $5.retlist;
							}

		| FOR LPAREN stmt m1 bexp SEMICOLON m1 stmt m5 RPAREN m1 stmt	{
							  if ($3.nextlist != NULL)
      								$3.nextlist->Backpatch($4.label);

							  $5.truelist->Backpatch($11.label);
							  enforce_valid_nextlist($12.nextlist);
							  $12.nextlist->Backpatch($7.label);

							  $9.dest->Merge($8.nextlist)->Backpatch($4.label);

				    			  $$.nextlist = $5.falselist;
							  $$.retlist = ($3.retlist != NULL) ? $3.retlist->Merge($8.retlist)->Merge($12.retlist) :
								       ($8.retlist != NULL) ? $8.retlist->Merge($12.retlist) :
								       $12.retlist;
							}

		| WHILE m1 bexp DO m1 stmt		{ $3.truelist->Backpatch($5.label);
							  $$.nextlist = $3.falselist;
							  $$.retlist = $6.retlist;
							  enforce_valid_nextlist($6.nextlist);
							  $6.nextlist->Backpatch($2.label);
							}
		;

bexp		: bexp OR m1 bterm			{ $$.truelist = $1.truelist->Merge($4.truelist);
							  $1.falselist->Backpatch($3.label);
							  $$.falselist = $4.falselist;
							}
		| bterm					{ $$.truelist = $1.truelist;
							  $$.falselist = $1.falselist;
							}
		;

bterm		: bterm AND m1 bfac			{ $1.truelist->Backpatch($3.label);
							  $$.truelist = $4.truelist;
							  $$.falselist = $1.falselist->Merge($4.falselist);
							}
		| bfac					{ $$.truelist = $1.truelist;
							  $$.falselist = $1.falselist;
							}
		;

bfac		: NOT bfac				{ $$.truelist = $2.falselist;
							  $$.falselist = $2.truelist;
							}
		| bpfac					{ $$.truelist = $1.truelist;
							  $$.falselist = $1.falselist;
							}
		;

bpfac		: exp RELOP exp				{ $$ = bpfac_relOp($2, $1.place, $3.place); }
		| LPAREN bexp RPAREN			{ $$.truelist = $2.truelist;
							  $$.falselist = $2.falselist;
							}
		;

exp		: CALL LPAREN exp COMMA exp RPAREN	{ $$.place = exp_call((CSymbol*[]){ $3.place, $5.place }); }
		| exp ADDOP term			{ $$.place = arithOp($2, $1.place, $3.place); }
		| term					{ $$.place = $1.place; }
		;

term		: term MULOP factor			{ $$.place = arithOp($2, $1.place, $3.place); }
		| factor				{ $$.place = $1.place; }
		;

factor		: LPAREN exp RPAREN			{ $$.place = $2.place; }
		| ID					{ $$.place = currentScope->Get($1); }
		| NUM					{ $$.place = newConst($1); }
		;

	/* markers */
m0		:					{ blk_enter(); }
		;

m1		:					{ $$.label = newLabel("m");
							  emit.label($$.label);
							}
		;

m2		:					{ $$.skip_label = newLabel("skip");
							  $$.prologue_label = newLabel("prologue-splice");
							  emit.ujump($$.skip_label);
							  emit.label($$.prologue_label);
							}
		;

m3		:					{ program_init(); }
		;

m4		:					{ main_init(); }
		;

m5		:					{ CSymbol *dest_label = newLabel("_____");
							  emit.ujump(dest_label);
							  $$.dest = new CBPList(dest_label);
							}
		;

m6		:					{ if ( stmt_has_nextlist ) {
								stmt_has_nextlist = false;
								$$.label = newLabel("m");
								emit.label($$.label);
							  }
							  else {
								$$.label = NULL;
							  }
							}
		;

%%
	/* user code */
void yyerror(char const *message)
{
	fprintf(stderr, "Input Parser [line %d]: %s", yylineno, message);
	if (yytext != NULL) {
		fprintf(stderr, ": %s", yytext);
	}
	fprintf(stderr, "\n");
	exit(1);
}

