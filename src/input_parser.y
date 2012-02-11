
	/* definitions */
%{

#include <stdio.h>
#include "utils.h"
#include "semantic_rules.h"
#include "symbol.h"

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
	SM1		_m1;
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

%start program
//-%type <node> function
// %type <node> main_function
%type <_blk> blk
//-%type <node> func_ret
//-%type <node> func_arg
%type <typeID> type
// %type <node> declarations
%type <_list> list
// %type <node> declarlist
// %type <node> dcl
%type <idQueue> idents
%type <_stmt> stmt
// %type <node> assn
%type <_cntrl> cntrl
//-%type <node> read
//-%type <node> write
//-%type <node> return
%type <_exp> exp
%type <_bexp> bexp
%type <_bterm> bterm
%type <_bfac> bfac
%type <_bpfac> bpfac
%type <_term> term
%type <_factor> factor

%type <_m1> m1

%right IF THEN ELSE

%%
	/* semantic rules */

program		: function main_function		{ currentScope->Disp();
							  emit.Disp();
							  return 0;
							}
		;

main_function	: PROGRAM blk				{ if ($2.nextlist != NULL) {
								CSymbol *label = newLabel("halt");
								emit.label(label);
								$2.nextlist->Backpatch(label);
							  }
							  emit.halt();
							}
		;

function	: func_ret func_arg blk			{ BUG_IF(true, "Non-implemented case"); }
		| /* eps */				{ }
		;

func_arg	: LPAREN ID COLON type COMMA ID COLON type RPAREN	{
						    	  BUG_IF(true, "Non-implemented case");
							}
		;

func_ret	: type FUNCTION				{ BUG_IF(true, "Non-implemented case"); }
		;

blk		: m0 declarations BEGIN_ list END	{ $$.nextlist = $4.nextlist;
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

list		: list m1 stmt				{ if ($1.nextlist != NULL)
								$1.nextlist->Backpatch($2.label);
							  $$.nextlist = $3.nextlist;
							}
		| /* eps */				{ $$.nextlist = NULL; }
		;

stmt		: assn					{ $$.nextlist = NULL; }
		| cntrl					{ $$.nextlist = $1.nextlist; }
		| read					{ BUG_IF(true, "Non-implemented case"); }
		| write					{ BUG_IF(true, "Non-implemented case"); }
		| return				{ BUG_IF(true, "Non-implemented case"); }
		| blk					{ BUG_IF(true, "Non-implemented case"); }
		;

write		: WRITE LPAREN exp RPAREN SEMICOLON	{ BUG_IF(true, "Non-implemented case");
							}
		;

return		: RETURN LPAREN exp RPAREN SEMICOLON	{ BUG_IF(true, "Non-implemented case");
							}
		;

read		: READ LPAREN ID RPAREN SEMICOLON	{ BUG_IF(true, "Non-implemented case");
							}
		;

assn		: ID ASSIGN exp SEMICOLON		{ emit.copy(currentScope->Get($1), $3.place); }
		;

cntrl		: IF bexp THEN m1 stmt ELSE m1 stmt	{ BUG_IF(true, "Non-implemented case"); }
		| IF bexp THEN m1 stmt			{ $2.truelist->Backpatch($4.label);
							  if ($5.nextlist == NULL) {
								CSymbol *next = newLabel("_____");
								emit.ujump(next);
								$5.nextlist = new CBPList(next);
							  }
							  $$.nextlist = $2.falselist->Merge($5.nextlist);
							}

		| FOR LPAREN stmt bexp SEMICOLON stmt RPAREN stmt	{
				    			  BUG_IF(true, "Non-implemented case");
							}
		| WHILE bexp DO stmt			{ BUG_IF(true, "Non-implemented case"); }
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

exp		: CALL LPAREN exp COMMA exp RPAREN	{ BUG_IF(true, "Non-implemented case"); }
		| exp ADDOP term			{ $$.place = newTemp(DestSymbolType($1.place, $3.place));
							  emit.arith($2, $$.place, $1.place, $3.place);
							}
		| term					{ $$.place = $1.place; }
		;

term		: term MULOP factor			{ $$.place = newTemp(DestSymbolType($1.place, $3.place));
							  emit.arith($2, $$.place, $1.place, $3.place);
							}
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

