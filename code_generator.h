#ifndef __CODE_GENERATOR_H__
#define __CODE_GENERATOR_H__

#include "token.h"

int codeGenerator(TokenList, FILE*);

void printCGErr(int errCode, FILE*);

#endif