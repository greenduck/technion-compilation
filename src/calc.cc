#include <stdlib.h>
#include <ctype.h>
#include "calc.h"
#include "utils.h"
#include "symbol.h"

string calc(string expr)
{
	string command;
	char ans[128];
	int i;

	command += "echo \"";
	command += expr;
	command += "\" | bc";

	FILE *fp = popen(command.c_str(), "r");
	unsigned count = fread(ans, 1, sizeof(ans), fp);
	BUG_IF((count >= sizeof(ans)), "Unexpectedly large numeric calculation result");
	pclose(fp);

	// fread() might pull in a few unrelated characters
	for (i = 0; isdigit(ans[i]); ++i);
	ans[i] = '\0';

//	dbgout << "calc: " << expr << " = " << ans << " \n";
	return string(ans);
}

