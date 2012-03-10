#include <stdlib.h>
#include <iostream>
#include <fstream>
#include "calc.h"
#include "utils.h"
#include "symbol.h"

string calc(string expr)
{
	const char *tempfile = "/tmp/calc";
	string command;
	string ans;

	command += "echo \"";
	command += expr;
	command += "\" | bc > ";
	command += tempfile;

	int errcode = system(command.c_str());
	BUG_IF((errcode != 0), "Executing external command failed: '" + command + "': " + string(CSymbol::NumericToString(errcode)));

	ifstream tmpf(tempfile, ifstream::in);
	tmpf >> ans;
	tmpf.close();

	return ans;
}

