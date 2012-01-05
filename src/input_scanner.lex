
	/* options */
%option nomain
%option yylineno
%option noyywrap
%option nounput

	/* definitions */
%{

#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "input_parser.h"

void scanner_error(const char *message);
void eat_up_comments();

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

"else"			{ return ELSE;
				}
"real"			{ return REAL;
				}
"integer"		{ return INTEGER;
				}
"write"			{ return WRITE;
				}
"while"			{ return WHILE;
				}
"end"			{ return END;
				}
"do"			{ return DO;
				}
"if"			{ return IF;
				}
"then"			{ return THEN;
				}
"program"		{ return PROGRAM;
				}
"function"		{ return FUNCTION;
				}
"return"		{ return RETURN;
				}
"read"			{ return READ;
				}
"var"			{ return VAR;
				}
"for"			{ return FOR;
				}
"begin"			{ return BEGIN_;
				}
"call"			{ return CALL;
				}

"("				{ return LPAREN;
				}
")"				{ return RPAREN;
				}
","				{ return COMMA;
				}
":"				{ return COLON;
				}
";"				{ return SEMICOLON;
				}

{id}			{ return ID;
				}

{whitespace}	{ /* ignore */
				}

.				{ scanner_error("undefined symbol");
				}
				
<<EOF>>			{ yytext = NULL;
				  scanner_error("unexpected end of file");
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

