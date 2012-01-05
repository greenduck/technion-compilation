
	/* definitions */
%{

#include "common.h"

int yylex(void);
void yyerror(const char *);

extern int yylineno;
extern char *yytext;

%}

%union {
	NODE node;
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

%token <node> NUM
%token <node> ID
%token <node> ADDOP
%token <node> MULOP
%token <node> RELOP

%start program
%type <node> function
%type <node> main_function
%type <node> blk
%type <node> func_ret
%type <node> func_arg
%type <node> type
%type <node> declarations
%type <node> list
%type <node> declarlist
%type <node> dcl
%type <node> idents
%type <node> stmt
%type <node> assn
%type <node> cntrl
%type <node> read
%type <node> write
%type <node> return
%type <node> exp
%type <node> bexp
%type <node> bterm
%type <node> bfac
%type <node> bpfac
%type <node> term
%type <node> factor

%right IF THEN ELSE

%%
	/* semantic rules */

program		: function main_function		{ printf("parsing complete \n");
							  return 0;
							}
		;

main_function	: PROGRAM blk				{}
		;

function	: func_ret func_arg blk			{}
		| /* eps */				{}
		;

func_arg	: LPAREN ID COLON type COMMA ID COLON type RPAREN	{}
		;

func_ret	: type FUNCTION				{}
		;

blk		: declarations BEGIN_ list END		{}
		;

declarations	: VAR declarlist			{}
		| /* eps */				{}
		;

declarlist	: dcl declarlist			{}
		| /* eps */				{}
		;

dcl		: idents COLON type SEMICOLON		{}
		;

idents		: ID					{}
		| idents COMMA ID			{}
		;

type		: INTEGER				{}
		| REAL					{}
		;

list		: list stmt				{}
		| /* eps */				{}
		;

stmt		: assn					{}
		| cntrl					{}
		| read					{}
		| write					{}
		| return				{}
		| blk					{}
		;

write		: WRITE LPAREN exp RPAREN SEMICOLON	{}
		;

return		: RETURN LPAREN exp RPAREN SEMICOLON	{}
		;

read		: READ LPAREN ID RPAREN SEMICOLON	{}
		;

assn		: ID ASSIGN exp SEMICOLON		{}
		;

cntrl		: IF bexp THEN stmt ELSE stmt		{}
		| IF bexp THEN stmt			{}
		| FOR LPAREN stmt bexp SEMICOLON stmt RPAREN stmt	{}
		| WHILE bexp DO stmt			{}
		;

bexp		: bexp OR bterm				{}
		| bterm					{}
		;

bterm		: bterm AND bfac			{}
		| bfac					{}
		;

bfac		: NOT bfac				{}
		| bpfac					{}
		;

bpfac		: exp RELOP exp				{}
		| LPAREN bexp RPAREN			{}
		;

exp		: CALL LPAREN exp COMMA exp RPAREN	{}
		| exp ADDOP term			{}
		| term					{}
		;

term		: term MULOP factor			{}
		| factor				{}
		;

factor		: LPAREN exp RPAREN			{}
		| ID					{}
		| NUM					{}
		;

%%
	/* user code */
int main()
{
	return yyparse();
}

void yyerror(char const *message)
{
	fprintf(stderr, "Input Parser [line %d]: %s", yylineno, message);
	if (yytext != NULL) {
		fprintf(stderr, ": %s", yytext);
	}
	fprintf(stderr, "\n");
	exit(1);
}

