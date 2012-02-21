#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>
#include <fstream>
#include "utils.h"
#include "semantic_rules.h"

static void parse_cmdline_args(int argc, char *argv[]);



int main(int argc, char *argv[])
{
	parse_cmdline_args(argc, argv);
	try {
		yyparse();

		currentScope->Disp();
		emit.Disp();
	}
	catch (CCompilationException& ex) {
		cerr << "Compile error [line " << yylineno << "]: " << ex << endl;
		return 1;
	}
	catch (CBugException& ex) {
		cerr << "Internal bug " << ex << endl;
		return 3;
	}

	return 0;
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

