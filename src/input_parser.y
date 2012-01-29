
	/* definitions */
%{

#include <stdio.h>
#include <string.h>
#include <libgen.h>
#include "utils.h"
#include "common.h"

#include <fstream>

int yylex(void);
int yyparse(void);
void yyerror(const char *);

extern int yylineno;
extern char *yytext;
extern FILE *yyin;

static void parse_cmdline_args(int argc, char *argv[]);

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
		| /* eps */				{ NODEPTR eps = make_node("EPSILON", NULL, NULL);
							  $$ = make_node("FUNCTION", NULL, eps);
							}
		;

func_arg	: LPAREN ID COLON type COMMA ID COLON type RPAREN	{
						    	  NODEPTR lpar = make_node("(", NULL, NULL);
							  NODEPTR id_0 = make_node("id", $2, NULL);
							  NODEPTR colon_0 = make_node(":", NULL, NULL);
							  NODEPTR comma = make_node(",", NULL, NULL);
							  NODEPTR id_1 = make_node("id", $6, NULL);
							  NODEPTR colon_1 = make_node(":", NULL, NULL);
							  NODEPTR rparen = make_node(")", NULL, NULL);
							  NODEPTR p = concat_list(lpar, id_0);
							  p = concat_list(p, colon_0);
							  p = concat_list(p, $4);
							  p = concat_list(p, comma);
							  p = concat_list(p, id_1);
							  p = concat_list(p, colon_1);
							  p = concat_list(p, $8);
							  p = concat_list(p, rparen);
							  $$ = make_node("FUNC_ARG", NULL, p);
							}
		;

func_ret	: type FUNCTION				{ NODEPTR func = make_node("function", NULL, NULL);
							  NODEPTR p = concat_list($1, func);
							  $$ = make_node("FUNC_RET", NULL, p);
							}
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
		| /* eps */				{ NODEPTR eps = make_node("EPSILON", NULL, NULL);
							  $$ = make_node("DECLARATIONS", NULL, eps);
							}
		;

declarlist	: dcl declarlist			{ NODEPTR p = concat_list($1, $2);
							  $$ = make_node("DECLARLIST", NULL, p);
							}
		| /* eps */				{ NODEPTR eps = make_node("EPSILON", NULL, NULL);
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
		| /* eps */				{ NODEPTR eps = make_node("EPSILON", NULL, NULL);
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

assn		: ID ASSIGN exp SEMICOLON		{ NODEPTR ret = make_node("id", $1, NULL);
							  NODEPTR ass = make_node("assign", NULL, NULL);
							  NODEPTR semicol = make_node(";", NULL, NULL);
							  NODEPTR p = concat_list(ret, ass);
							  p = concat_list(p, $3);
							  p = concat_list(p, semicol);
							  $$ = make_node("ASSN", NULL, p);
							}
		;

cntrl		: IF bexp THEN stmt ELSE stmt		{ NODEPTR iflex = make_node("if", NULL, NULL);
							  NODEPTR thenlex = make_node("then", NULL, NULL);
							  NODEPTR elselex = make_node("else", NULL, NULL);
							  NODEPTR p = concat_list(iflex, $2);
							  p = concat_list(p, thenlex);
							  p = concat_list(p, $4);
							  p = concat_list(p, elselex);
							  p = concat_list(p, $6);
							  $$ = make_node("CNTRL", NULL, p);
							}
		| IF bexp THEN stmt			{ NODEPTR iflex = make_node("if", NULL, NULL);
							  NODEPTR thenlex = make_node("then", NULL, NULL);
							  NODEPTR p = concat_list(iflex, $2);
							  p = concat_list(p, thenlex);
							  p = concat_list(p, $4);
							  $$ = make_node("CNTRL", NULL, p);
							}
		| FOR LPAREN stmt bexp SEMICOLON stmt RPAREN stmt	{
				    			  NODEPTR forlex = make_node("for", NULL, NULL);
							  NODEPTR lpar = make_node("(", NULL, NULL);
							  NODEPTR semicol = make_node(";", NULL, NULL);
							  NODEPTR rpar = make_node(")", NULL, NULL);
							  NODEPTR p = concat_list(forlex, lpar);
							  p = concat_list(p, $3);
							  p = concat_list(p, $4);
							  p = concat_list(p, semicol);
							  p = concat_list(p, $6);
							  p = concat_list(p, rpar);
							  p = concat_list(p, $8);
							  $$ = make_node("CNTRL", NULL, p);
							}
		| WHILE bexp DO stmt			{ NODEPTR whilelex = make_node("while", NULL, NULL);
							  NODEPTR dolex = make_node("do", NULL, NULL);
							  NODEPTR p = concat_list(whilelex, $2);
							  p = concat_list(p, dolex);
							  p = concat_list(p, $4);
							  $$ = make_node("CNTRL", NULL, p);
							}
		;

bexp		: bexp OR bterm				{ NODEPTR orlex = make_node("or", NULL, NULL);
							  NODEPTR p = concat_list($1, orlex);
							  p = concat_list(p, $3);
							  $$ = make_node("BEXP", NULL, p);
							}
		| bterm					{ $$ = make_node("BEXP", NULL, $1); }
		;

bterm		: bterm AND bfac			{ NODEPTR andlex = make_node("and", NULL, NULL);
							  NODEPTR p = concat_list($1, andlex);
							  p = concat_list(p, $3);
							  $$ = make_node("BTERM", NULL, p);
							}
		| bfac					{ $$ = make_node("BTERM", NULL, $1); }
		;

bfac		: NOT bfac				{ NODEPTR notlex = make_node("not", NULL, NULL);
							  NODEPTR p = concat_list(notlex, $2);
							  $$ = make_node("BFAC", NULL, p);
							}
		| bpfac					{ $$ = make_node("BFAC", NULL, $1); }
		;

bpfac		: exp RELOP exp				{ NODEPTR relop = make_node("relop", $2, NULL);
							  NODEPTR p = concat_list($1, relop);
							  p = concat_list(p, $3);
							  $$ = make_node("BPFAC", NULL, p);
							}
		| LPAREN bexp RPAREN			{ NODEPTR lpar = make_node("(", NULL, NULL);
							  NODEPTR rpar = make_node(")", NULL, NULL);
							  NODEPTR p = concat_list(lpar, $2);
							  p = concat_list(p, rpar);
							  $$ = make_node("BPFAC", NULL, p);
							}
		;

exp		: CALL LPAREN exp COMMA exp RPAREN	{ NODEPTR call = make_node("call", NULL, NULL);
							  NODEPTR lpar = make_node("(", NULL, NULL);
							  NODEPTR comma = make_node(",", NULL, NULL);
							  NODEPTR rpar = make_node(")", NULL, NULL);
							  NODEPTR p = concat_list(call, lpar);
							  p = concat_list(p, $3);
							  p = concat_list(p, comma);
							  p = concat_list(p, $5);
							  p = concat_list(p, rpar);
							  $$ = make_node("EXP", NULL, p);
							}
		| exp ADDOP term			{ NODEPTR addop = make_node("addop", $2, NULL);
							  NODEPTR p = concat_list($1, addop);
							  p = concat_list(p, $3);
							  $$ = make_node("EXP", NULL, p);
							}
		| term					{ $$ = make_node("EXP", NULL, $1); }
		;

term		: term MULOP factor			{ NODEPTR mulop = make_node("mulop", $2, NULL);
							  NODEPTR p = concat_list($1, mulop);
							  p = concat_list(p, $3);
							  $$ = make_node("TERM", NULL, p);
							}
		| factor				{ $$ = make_node("TERM", NULL, $1); }
		;

factor		: LPAREN exp RPAREN			{ NODEPTR lpar = make_node("(", NULL, NULL);
							  NODEPTR rpar = make_node(")", NULL, NULL);
							  NODEPTR p = concat_list(lpar, $2);
							  p = concat_list(p, rpar);
							  $$ = make_node("FACTOR", NULL, p);
							}
		| ID					{ NODEPTR id = make_node("id", $1, NULL);
							  $$ = make_node("FACTOR", NULL, id);
							}
		| NUM					{ NODEPTR num = make_node("num", $1, NULL);
							  $$ = make_node("FACTOR", NULL, num);
							}
		;

%%
	/* user code */
int main(int argc, char *argv[])
{
	parse_cmdline_args(argc, argv);
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

static void print_usage(const char *name)
{
	printf("Usage: \n" \
	       "%s [-help] [-dbg <debug-file>] [-const] [input-file] [output-file] \n",
	       name);
	exit(1);
}

static void parse_cmdline_args(int argc, char *argv[])
{
	int i;
	bool const_opt = false;
	char *infile = NULL;
	char *outfile = NULL;
	char *dbgfile = NULL;

	for (i = 1; i < argc; ++i) {
		if (!strcmp("-help", argv[i])) {
			print_usage(basename(argv[0]));
		}

		if (!strcmp("-dbg", argv[i])) {
			if (i + 1 < argc) {
				dbgfile = argv[++i];
			}
			continue;
		}

		if (!strcmp("-const", argv[i])) {
			const_opt = true;
			continue;
		}

		if (infile == NULL) {
			infile = argv[i];
			continue;
		}

		if (outfile == NULL) {
			outfile = argv[i];
			continue;
		}

		print_usage(basename(argv[0]));
	}

	// redirect input: stdin -> infile
	if (infile != NULL) {
		if ((yyin = fopen(infile, "r")) == NULL) {
			fprintf(stderr, "Could not open input file \n");
			exit(1);
		}
	}

	// redirect output: stdout -> outfile
	if (outfile != NULL) {
		filebuf *fb = new(filebuf);	// no delete
		fb->open(outfile, ios_base::out | ios_base::trunc);
		machout.rdbuf(fb);
	}

	if (dbgfile != NULL) {
		filebuf *fb = new(filebuf);	// no delete
		fb->open(dbgfile, ios_base::out | ios_base::trunc);
		dbgout.rdbuf(fb);
	}
}

