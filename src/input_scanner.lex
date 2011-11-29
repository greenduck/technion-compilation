
/*
 * Lexer
 * Stand-alone build:
 * flex input_scanner.lex
 * gcc -Wall -o test lex.yy.c
 * Test run:
 * cat test.txt | ./test
 * Reference:
 * http://flex.sourceforge.net/manual/index.html
 */

	/* options */
%option nomain
%option yylineno
%option noyywrap
%option nounput

	/* definitions */
%{

#include <stdio.h>
#include <stdlib.h>

void scanner_error(const char *message);
void eat_up_comments();
int main();

enum {
	RES=0,
	PUNCT,
	ID,
	NUM,
	RELOP,
	ADDOP,
	MULOP,
	ASSIGN,
	AND,
	OR,
	NOT,
	
	END_OF_INPUT
};

static const char *lexeme_to_string[] = {
	"res",
	NULL,
	"id",
	"num",
	"relop",
	"addop",
	"mulop",
	"assign",
	"and",
	"or",
	"not"
};

%}

digit		[0-9]
integer		{digit}+
real		{integer}"."{digit}+
num			{integer}|{real}

letter		[a-zA-Z]
alpha_num	{letter}|{digit}
id			{letter}{alpha_num}*

relop		"=="|"<>"|"<"|"<="|">"|">="
addop		[+-]
mulop		[*/]

whitespace	[ \t\n\r]+


%%
	/* rules */
"/*"			{ eat_up_comments();
				}

{num}			{ return NUM;
				}
{relop}			{ return RELOP;
				}
{addop}			{ return ADDOP;
				}
{mulop}			{ return MULOP;
				}
"="				{ return ASSIGN;
				}
"&&"			{ return AND;
				}
"||"			{ return OR;
				}
"!"				{ return NOT;
				}

"else"			{ return RES;
				}
"real"			{ return RES;
				}
"integer"		{ return RES;
				}
"write"			{ return RES;
				}
"while"			{ return RES;
				}
"end"			{ return RES;
				}
"do"			{ return RES;
				}
"if"			{ return RES;
				}
"then"			{ return RES;
				}
"program"		{ return RES;
				}
"function"		{ return RES;
				}
"return"		{ return RES;
				}
"read"			{ return RES;
				}
"var"			{ return RES;
				}
"for"			{ return RES;
				}
"begin"			{ return RES;
				}
"call"			{ return RES;
				}

"("				{ return PUNCT;
				}
")"				{ return PUNCT;
				}
","				{ return PUNCT;
				}
":"				{ return PUNCT;
				}
";"				{ return PUNCT;
				}
"."				{ return PUNCT;
				}

{id}			{ return ID;
				}

{whitespace}	{ printf("%s", yytext);	/* FIXME: ignore */
				}

.				{ scanner_error("undefined symbol");
				}
				
<<EOF>>			{ return END_OF_INPUT;
				}


%%
	/* user code */
void scanner_error(const char *message)
{
	fprintf(stderr, "Lexical Error [line %d]: %s", yylineno, message);
	if (yytext != NULL) {
		fprintf(stderr, ": %s", yytext);
	}
	fprintf(stderr, "\n");
	exit(1);
}

#ifndef __cplusplus
#define yyinput input
#endif
void eat_up_comments()
{
	int c;
	
	while (1) {
		while ((c = yyinput()) != '*' && c != EOF);
		
		if (c == '*') {
			while ((c = yyinput()) == '*');
			if (c == '/') {
				/* end of comment */
				return;
			}
		}
		
		if (c == EOF) {
			yytext = NULL;
			scanner_error("open comment at end of file");
		}
	}
}

// testing
int main()
{
	int lexeme_code;
	const char *lexeme_str;
	
	while ((lexeme_code = yylex()) != END_OF_INPUT) {
		lexeme_str = lexeme_to_string[lexeme_code];
		if (lexeme_str != NULL) {
			printf("<%s,%s>", lexeme_str, yytext);
		}
		else {
			printf("%s", yytext);
		}
	}
	printf("\n");
	
	return 0;
}

