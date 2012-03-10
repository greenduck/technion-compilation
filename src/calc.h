/*
 * Calculator needed for compile-time expression evaluation (constant propagation) 
 * Current implementation does not _really_ implement a calculator, rather relying 
 * on shell 'bc' command, making the whole compiler Unix-only. 
 */
#ifndef _CALC_H
#define _CALC_H

#include <string>

using namespace std;

string calc(string expr);

#endif	// _CALC_H

