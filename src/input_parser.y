
	/* definitions */
%{

#include "common.h"

int yylex(void);
int yyparse(void);
void yyerror(const char *);

extern int yylineno;
extern char *yytext;

%}

%union {
	char *text;
	NODEPTR node;
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

program		: function main_function		{ NODEPTR p = concat_list($1, $2);
							  NODEPTR all_prog = make_node("PROGRAM", NULL, p);
							  traverse_dfs(all_prog);
							  printf("\n");
							  return 0;
							}
		;

main_function	: PROGRAM blk				{ NODEPTR prog = make_node("program", NULL, NULL);
							  NODEPTR p = concat_list(prog, $2);
							  $$ = make_node("MAIN_FUNCTION", NULL, p);
							}
		;

function	: func_ret func_arg blk			{ NODEPTR p = concat_list($1, $2);
							  p = concat_list(p, $3);
							  $$ = make_node("FUNCTION", NULL, p);
							}
		| /* eps */				{ NODEPTR eps = make_node("epsilon", NULL, NULL);
							  $$ = make_node("FUNCTION", NULL, eps);
							}
		;

func_arg	: LPAREN ID COLON type COMMA ID COLON type RPAREN	{}
		;

func_ret	: type FUNCTION				{}
		;

blk		: declarations BEGIN_ list END		{ NODEPTR beg = make_node("begin", NULL, NULL);
							  NODEPTR end = make_node("end", NULL, NULL);
							  NODEPTR p = concat_list($1, beg);
							  p = concat_list(p, $3);
							  p = concat_list(p, end);
							  $$ = make_node("BLK", NULL, p);
							}
		;

declarations	: VAR declarlist			{ NODEPTR var = make_node("var", NULL, NULL);
							  NODEPTR p = concat_list(var, $2);
      							  $$ = make_node("DECLARATIONS", NULL, p);
							}
		| /* eps */				{ NODEPTR eps = make_node("epsilon", NULL, NULL);
							  $$ = make_node("DECLARATIONS", NULL, eps);
							}
		;

declarlist	: dcl declarlist			{ NODEPTR p = concat_list($1, $2);
							  $$ = make_node("DECLARLIST", NULL, p);
							}
		| /* eps */				{ NODEPTR eps = make_node("epsilon", NULL, NULL);
							  $$ = make_node("DECLARLIST", NULL, eps);
							}
		;

dcl		: idents COLON type SEMICOLON		{ NODEPTR col = make_node(":", NULL, NULL);
							  NODEPTR semicol = make_node(";", NULL, NULL);
							  NODEPTR p = concat_list($1, col);
							  p = concat_list(p, $3);
							  p = concat_list(p, semicol);
							  $$ = make_node("DCL", NULL, p);
							}
		;

idents		: ID					{ NODEPTR id = make_node("id", $1, NULL);
							  $$ = make_node("IDENTS", NULL, id);
							}
		| idents COMMA ID			{ NODEPTR comma = make_node(",", NULL, NULL);
							  NODEPTR id = make_node("id", $3, NULL);
							  NODEPTR p = concat_list($1, comma);
							  p = concat_list(p, id);
							  $$ = make_node("IDENTS", NULL, p);
							}
		;

type		: INTEGER				{ NODEPTR integer = make_node("integer", NULL, NULL);
							  $$ = make_node("TYPE", NULL, integer);
							}
		| REAL					{ NODEPTR real = make_node("real", NULL, NULL);
							  $$ = make_node("TYPE", NULL, real);
							}
		;

list		: list stmt				{ NODEPTR p = concat_list($1, $2);
							  $$ = make_node("LIST", NULL, p);
							}
		| /* eps */				{ NODEPTR eps = make_node("epsilon", NULL, NULL);
							  $$ = make_node("LIST", NULL, eps);
							}
		;

stmt		: assn					{ $$ = make_node("STMT", NULL, $1); }
		| cntrl					{ $$ = make_node("STMT", NULL, $1); }
		| read					{ $$ = make_node("STMT", NULL, $1); }
		| write					{ $$ = make_node("STMT", NULL, $1); }
		| return				{ $$ = make_node("STMT", NULL, $1); }
		| blk					{ $$ = make_node("STMT", NULL, $1); }
		;

write		: WRITE LPAREN exp RPAREN SEMICOLON	{ NODEPTR wri = make_node("write", NULL, NULL);
							  NODEPTR lpar = make_node("(", NULL, NULL);
							  NODEPTR rpar = make_node(")", NULL, NULL);
							  NODEPTR semicol = make_node(";", NULL, NULL);
							  NODEPTR p = concat_list(wri, lpar);
							  p = concat_list(p, $3);
							  p = concat_list(p, rpar);
							  p = concat_list(p, semicol);
							  $$ = make_node("WRITE", NULL, p);
							}
		;

return		: RETURN LPAREN exp RPAREN SEMICOLON	{ NODEPTR ret = make_node("return", NULL, NULL);
							  NODEPTR lpar = make_node("(", NULL, NULL);
							  NODEPTR rpar = make_node(")", NULL, NULL);
							  NODEPTR semicol = make_node(";", NULL, NULL);
							  NODEPTR p = concat_list(ret, lpar);
							  p = concat_list(p, $3);
							  p = concat_list(p, rpar);
							  p = concat_list(p, semicol);
							  $$ = make_node("RETURN", NULL, p);
							}
		;

read		: READ LPAREN ID RPAREN SEMICOLON	{ NODEPTR red = make_node("read", NULL, NULL);
							  NODEPTR lpar = make_node("(", NULL, NULL);
							  NODEPTR id = make_node("id", $3, NULL);
							  NODEPTR rpar = make_node(")", NULL, NULL);
							  NODEPTR semicol = make_node(";", NULL, NULL);
							  NODEPTR p = concat_list(red, lpar);
							  p = concat_list(p, id);
							  p = concat_list(p, rpar);
							  p = concat_list(p, semicol);
							  $$ = make_node("READ", NULL, p);
							}
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

